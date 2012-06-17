/* Part of publib.

   Copyright (c) 1994-2006 Lars Wirzenius.  All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*
 * tbuf.c -- implement text editor buffers using trees
 *
 * Part of Publib, see man page for more information.
 * "@(#)publib-tbuf:$Id: tbuf.c,v 1.12 2003/11/15 18:24:11 liw Exp $"
 *
 * 
 * A brief description of how this works.
 * XXX probably needs to be updated
 * 
 * The text of a buffer is broken down into `chunks', which contain up to
 * CHUNK_MAX characters each. The chunks are kept in a B-tree -like tree,
 * as leaf nodes. Chunks and trees are not modified after they have been
 * created. Instead, if the buffer needs to be modified, a copy is made,
 * but those parts of the buffer that have not been modifed are shared
 * between the original and the new copy.
 * 
 * For example, assume we have the following tree, representing the buffer
 * "abcdef" (with CHUNK_MAX being 3):
 * 
 * 	  {=]
 * 	  / \
 * 	abc def
 * 
 * ("[=]" is an interior node.) Now assume that we want to delete "abc" from
 * the buffer. We then create a new buffer that has a new interior node that
 * has only one child, the chunk that contains "def":
 * 
 * 	  {=]		[=]
 * 	  / \            :
 * 	abc def <......../
 * 
 * This way, the only new memory needed is that for the (small) interior node.
 * In a larger example some of the interior nodes would be shared as well.
 * 
 * When the original buffer is no longer needed, it can be destroyed. This
 * does not destroy the "def" chunk, since it is shared.
 * 
 * Similarly, when two buffers are catenated, the new tree uses as many
 * chunks and interior nodes of the old tree as possible. When sharing
 * is impossible, some nodes are copied and the copies are altered as
 * necessary.
 * 
 * We use reference counting to implement the sharing of nodes and chunks
 * between trees. When a node or chunk is created, it's reference count
 * is set to 1. When a new tree is created and a new node is created and
 * points at a node in the old tree, the reference count for the old node is
 * incremented. However, nodes and chunks below that node are not touched,
 * since the new node does not directly reference them. When a tree is
 * destroyed, reference counts are decremented and if they fall to zero,
 * the node or chunk is freed.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#include "publib/tbuf.h"


/*
 * Internal data structures.
 *
 * This has some potential improvement for the future: At the moment, all
 * chunks and nodes are allocated with separate calls to malloc, and at least
 * in some environments this is rather expensive, both in CPU time and memory
 * use. It would probably make sense to allocate lots of memory at once with
 * malloc, and then have a simpler custom memory allocator that gives out parts
 * of the large piece of malloc'd memory. At least for the chunk and node
 * structs, which are of a constant size; not necessarily for the text parts
 * of chunks.
 */

/* The maximum number of characters in a chunk. */
#define CHUNK_MAX 10 /* XXX */
#ifndef CHUNK_MAX
#define CHUNK_MAX 128
#endif

/* The chunk itself. */
typedef struct chunk Chunk;
struct chunk {
	Chunk *next;
	size_t length;
	char *chars;
	unsigned visited:1;
};


/* The maximum number of children a node may contain. */
#define NODE_MAX 3 /* XXX */
#ifndef NODE_MAX
#define NODE_MAX    8
#endif

/* The node itself. `height' is the level of this node, counting from leafs
   (level 1) upwards, so that a node having chunks as its children has a 
   height of 2. `length' is
   the length of all chunks in the subtree for which this node is root. */
typedef struct node Node;
struct node {
	Node *next;
	size_t height, length, kids;
	union {
		Chunk *chunk[NODE_MAX];
		Node *node[NODE_MAX];
	} x;
	unsigned visited:1;
};


/* The buffer as presented to the user. (Of course, the user never sees inside
   this structure. Bondage and discipline needs to be enforced, them program
   writing lusers are dangerous.)
   
   `height' is the height of the tree containing the buffer. If it is 0,
   the buffer is empty. If 1, the tree only needs a chunk, so use x.chunk.
   Otherwise, there is at least one node, so use x.root.
   
   `offset' is the offset where `our' part of the character sequence
   described by the tree begins. `length' is the number of characters
   that belong to this buffer after the offset. This allows us to implement
   tbuf_copy very cheaply: it does not need to modify any part of the tree,
   just create a new Tbuf with modified offset and length fields.  */
struct tbuf {
	Tbuf *next, *prev;
	size_t height, offset, length;
	union {
		Chunk *chunk;
		Node *root;
	} x;
};


/* List of all Tbufs, Nodes and Chunk that exist at the moment. Needed for 
   garbage collection. */
static Tbuf *tbuf_chain = NULL;
static Node *node_chain = NULL;
static Chunk *chunk_chain = NULL;

/* List of unused Nodes. */
static Node *free_nodes = NULL;


/*
 * Routines to print out the data structures for debugging. These are not
 * normally enabled.
 */

#if 0
#define dump_chunk(f, level, chunk) ((void) 0)
#define dump_node(f, level, node) ((void) 0)
#define dump_tbuf(f, tbuf) ((void) 0)
#else

static void dump_chunk(FILE *f, int level, Chunk *chunk) {
	int c, spaces;
	size_t i;
	
	spaces = level * 2;
	fprintf(f, "%*schunk at %p\n", spaces, "", (void *) chunk);
	fprintf(f, "%*s..length=%lu\n", spaces, "", (unsigned long) chunk->length);
	fprintf(f, "%*s..chars(%p)=<", spaces, "", (void *) chunk->chars);
	fflush(f);
	for (i = 0; i < chunk->length; ++i) {
		c = chunk->chars[i];
		if (isprint(c))
			fprintf(f, "%c", c);
		else
			fprintf(f, "<%02x>", (unsigned) c);
	}
	fprintf(f, ">\n");
}

static void dump_node(FILE *f, int level, Node *node) {
	int spaces;
	size_t i;
	
	spaces = level * 2;
	fprintf(f, "%*snode at %p\n", spaces, "", (void *) node);
	fprintf(f, "%*s..height=%lu\n", spaces, "", (unsigned long) node->height);
	fprintf(f, "%*s..length=%lu\n", spaces, "", (unsigned long) node->length);
	fprintf(f, "%*s..kids=%lu\n", spaces, "", (unsigned long) node->kids);
	
	if (node->height == 2) {
		for (i = 0; i < node->kids; ++i)
			dump_chunk(f, level + 1, node->x.chunk[i]);
	} else {
		for (i = 0; i < node->kids; ++i)
			dump_node(f, level + 1, node->x.node[i]);
	}
}

#if 0
static void dump_tbuf(FILE *f, Tbuf *tbuf) {
	fprintf(f, "tbuf at %p\n", (void *) tbuf);
	fprintf(f, "..height=%lu\n", (unsigned long) tbuf->height);
	fprintf(f, "..offset=%lu\n", (unsigned long) tbuf->offset);
	fprintf(f, "..length=%lu\n", (unsigned long) tbuf->length);
	if (tbuf->height == 1)
		dump_chunk(f, 1, tbuf->x.chunk);
	else if (tbuf->height >= 2)
		dump_node(f, 1, tbuf->x.root);
	fprintf(f, "end of tbuf dump at %p\n", (void *) tbuf);
}
#endif

#endif


/*
 * tbuf_create and the routines it needs. `new_chunk' is used elsewhere as
 * well.
 */

/* Create a new chunk. Advance `*chars' and reduce `*nchars' by the length
   of the newly created chunk. */
static Chunk *new_chunk(const char **chars, size_t *nchars) {
	size_t n;
	Chunk *chunk;

	if (*nchars <= CHUNK_MAX)
		n = *nchars;
	else
		n = CHUNK_MAX;

	chunk = malloc(sizeof(Chunk));
	if (chunk == NULL)
		return NULL;

	if (n == 0)
		chunk->chars = NULL;
	else {
		chunk->chars = malloc(n);
		if (chunk->chars == NULL) {
			free(chunk);
			return NULL;
		}
		memcpy(chunk->chars, *chars, n);
	}

	chunk->length = n;
	*chars += n;
	*nchars -= n;
	chunk->visited = 0;
	chunk->next = chunk_chain;
	chunk_chain = chunk;

	return chunk;
}

static Node *new_empty_node(void) {
	Node *node;

	if (free_nodes == NULL) {
		node = malloc(sizeof(Node));
		if (node == NULL)
			return NULL;
	} else {
		node = free_nodes;
		free_nodes = free_nodes->next;
	}
	node->next = node_chain;
	node_chain = node;
	node->height = 0;
	node->length = 0;
	node->kids = 0;
	node->visited = 0;
	
	return node;
}

/* Create a new node. `*chars' and `*nchars' as for new_chunk (surprisingly
   enough :). */
static Node *new_node(size_t height, const char **chars, size_t *nchars) {
	Node *node;
	size_t i;

	node = new_empty_node();
	if (node == NULL)
		return NULL;

	node->height = height;
	node->length = 0;
	node->visited = 0;

	if (height == 2) {
		for (i = 0; i < NODE_MAX && *nchars > 0; ++i) {
			node->x.chunk[i] = new_chunk(chars, nchars);
			if (node->x.chunk[i] == NULL)
				return NULL;
			node->length += node->x.chunk[i]->length;
		}
	} else {
		for (i = 0; i < NODE_MAX && *nchars > 0; ++i) {
			node->x.node[i] = new_node(height-1, chars, nchars);
			if (node->x.node[i] == NULL)
				return NULL;
			node->length += node->x.node[i]->length;
		}
	}
	node->kids = i;
	
	return node;
}

/* Build a tree of nodes and chunks. */
static void *build_tree(size_t *height, const char *chars, size_t nchars) {
	size_t max_length_for_height;

	assert(nchars > 0);
	*height = 1;
	max_length_for_height = CHUNK_MAX;
	while (max_length_for_height < nchars) {
		++*height;
		max_length_for_height *= NODE_MAX;
	}

	if (*height == 1)
		return new_chunk(&chars, &nchars);
	else
		return new_node(*height, &chars, &nchars);
}


static Tbuf *new_tbuf(void) {
	Tbuf *tbuf;
	
	tbuf = malloc(sizeof(Tbuf));
	if (tbuf == NULL)
		return NULL;
	tbuf->height = 0;
	tbuf->offset = 0;
	tbuf->length = 0;
	tbuf->next = tbuf_chain;
	tbuf->prev = NULL;
	if (tbuf_chain != NULL)
		tbuf_chain->prev = tbuf;
	tbuf_chain = tbuf;
	return tbuf;
}


Tbuf *tbuf_create(const char *chars, size_t nchars) {
	Tbuf *tbuf;
	void *p;

	tbuf = new_tbuf();

	if (nchars == 0) {
		p = NULL;
	} else {
		p = build_tree(&tbuf->height, chars, nchars);
		if (p == NULL) {
			tbuf_destroy(tbuf);
			return NULL;
		}
	}

	switch (tbuf->height) {
	case 0:
		tbuf->x.chunk = NULL;
		break;
	case 1:
		tbuf->x.chunk = p;
		break;
	default:
		tbuf->x.root = p;
		break;
	}

	tbuf->length = nchars;
	return tbuf;
}



/*
 * tbuf_destroy and the routines it needs.
 */

static void chunk_destroy(Chunk *chunk) {
	if (chunk != NULL) {
		free(chunk->chars);
		free(chunk);
	}
}

static void node_destroy(Node *node) {
	node->next = free_nodes;
	free_nodes = node;
}

static void node_zero_visited(Node *node) {
	size_t i;

	node->visited = 0;
	if (node->height == 2) {
		for (i = 0; i < node->kids; ++i)
			node->x.chunk[i]->visited = 0;
	} else {
		for (i = 0; i < node->kids; ++i)
			node_zero_visited(node->x.node[i]);
	}
}

static void garbage_collect(void) {
	Tbuf *tbuf;
	Node *node, *new_node_chain, *next_node;
	Chunk *chunk, *new_chunk_chain, *next_chunk;
	
	for (chunk = chunk_chain; chunk != NULL; chunk = chunk->next)
		chunk->visited = 1;
	for (node = node_chain; node != NULL; node = node->next)
		node->visited = 1;

	for (tbuf = tbuf_chain; tbuf != NULL; tbuf = tbuf->next) {
		switch (tbuf->height) {
		case 0:
			break;
		case 1:
			tbuf->x.chunk->visited = 0;
			break;
		default:
			node_zero_visited(tbuf->x.root);
		}
	}
	
	new_chunk_chain = NULL;
	for (chunk = chunk_chain; chunk != NULL; chunk = next_chunk) {
		next_chunk = chunk->next;
		if (chunk->visited == 1)
			chunk_destroy(chunk);
		else {
			chunk->next = new_chunk_chain;
			new_chunk_chain = chunk;
		}
	}
	chunk_chain = new_chunk_chain;
	
	new_node_chain = NULL;
	for (node = node_chain; node != NULL; node = next_node) {
		next_node = node->next;
		if (node->visited == 1)
			node_destroy(node);
		else {
			node->next = new_node_chain;
			new_node_chain = node;
		}
	}
	node_chain = new_node_chain;
}


void tbuf_destroy(Tbuf *tbuf) {
	assert(tbuf != NULL);
#if 1
	if (tbuf_chain == tbuf)
		tbuf_chain = tbuf->next;
	if (tbuf->prev != NULL)
		tbuf->prev->next = tbuf->next;
	if (tbuf->next != NULL)
		tbuf->next->prev = tbuf->prev;
	free(tbuf);
#endif
	garbage_collect();
}


/*
 * tbuf_chars and the routines it needs.
 */

/* Copy part of a chunk to a character array, and advance the pointer to the
   array by the amount copied. */
static void chunk_collect(char **chars, Chunk *chunk, size_t off, size_t n) {
	memcpy(*chars, chunk->chars + off, n);
	*chars += n;
}

/* Visit all relevant chunks and copy the relevant parts of them. Subtrees
   can sometimes be skipped completely, based on `*off' and their length. */
static void node_collect(char **chars, Node *node, size_t *off, size_t *n) {
	size_t i;
	Chunk *chunk;

	if (node->length <= *off)
		*off -= node->length;
	else if (node->height > 2) {
		for (i = 0; i < node->kids && *n > 0; ++i)
			node_collect(chars, node->x.node[i], off, n);
	} else {
		for (i = 0; i < node->kids && *n > 0; ++i) {
			chunk = node->x.chunk[i];
			if (chunk->length <= *off)
				*off -= chunk->length;
			else if (*n <= chunk->length) {
				chunk_collect(chars, chunk, *off, *n);
				*n = 0;
			} else {
				chunk_collect(chars, chunk, *off, chunk->length);
				*off = 0;
				*n -= chunk->length;
			}
		}
	}
}

void tbuf_chars(char *chars, Tbuf *tbuf, size_t start, size_t length) {
	assert(chars != NULL);
	assert(tbuf != NULL);

	if (start >= tbuf->length || length == 0)
		return;
	if (start + length > tbuf->length)
		length = tbuf->length - start;

	start += tbuf->offset;
	if (tbuf->height == 1)
		chunk_collect(&chars, tbuf->x.chunk, start, length);
	else
		node_collect(&chars, tbuf->x.root, &start, &length);
}



/*
 * tbuf_cat and stuff that it needs. This is the most difficult operation
 * and needs the largest amount of code. This needs pictures to be explained,
 * I think, and I don't have the time to draw them now.
 */

/* Create a new chunk out an old one. */
static Chunk *copy_chunk(Chunk *chunk, size_t offset, size_t length) {
	const char *p;
	
	assert(offset < chunk->length);
	assert(offset + length <= chunk->length);
	p = chunk->chars + offset;
	return new_chunk(&p, &length);
}

/* Create a new node out of an old one. */
/* NOTE: to is "up to", not inclusive. */
static Node *copy_node(Node *node, void *before, size_t from, size_t to,
			void *after) {
	Node *node_new;
	size_t i;
	
	node_new = new_empty_node();
	if (node_new == NULL)
		return NULL;

	node_new->height = node->height;
	node_new->length = 0;
	node_new->kids = 0;

	if (node_new->height == 2) {
		if (before != NULL)
			node_new->x.chunk[node_new->kids++] = before;
		for (i = from; i < to; ++i)
			node_new->x.chunk[node_new->kids++] = node->x.chunk[i];
		if (after != NULL)
			node_new->x.chunk[node_new->kids++] = after;
		for (i = 0; i < node_new->kids; ++i)
			node_new->length += node_new->x.chunk[i]->length;
	} else {
		if (before != NULL)
			node_new->x.node[node_new->kids++] = before;
		for (i = from; i < to; ++i)
			node_new->x.node[node_new->kids++] = node->x.node[i];
		if (after != NULL)
			node_new->x.node[node_new->kids++] = after;
		for (i = 0; i < node_new->kids; ++i)
			node_new->length += node_new->x.node[i]->length;
	}

	assert(node_new->kids > 0);
	return node_new;
}

/* Create a new node tree out of an old one that begins after `offset' in the
   old one. */
static Node *chop_before(Node *root, size_t offset) {
	Node *node, *node_new, *root_new;
	Chunk *chunk, *chunk_new;
	size_t i, orig_offset;

	if (offset == 0)
		return root;

	orig_offset = offset;
	if (root->height == 2) {
		for (i = 0; ; ++i) {
			assert(i < root->kids);
			chunk = root->x.chunk[i];
			if (chunk->length >= offset)
				break;
			offset -= chunk->length;
		}
		if (offset == chunk->length)
			chunk_new = NULL;
		else {
			chunk_new = copy_chunk(chunk, offset, chunk->length - offset);
			if (chunk_new == NULL)
				return NULL;
		}
		root_new = copy_node(root, chunk_new, i+1, root->kids, NULL);
		if (root_new == NULL)
			return NULL;
		assert(root_new->length == root->length - orig_offset);
	} else {
		for (i = 0; ; ++i) {
			assert(i < root->kids);
			node = root->x.node[i];
			if (node->length >= offset)
				break;
			offset -= node->length;
		}
		if (offset == node->length)
			node_new = NULL;
		else {
			node_new = chop_before(node, offset);
			if (node_new == NULL)
				return NULL;
		}
		root_new = copy_node(root, node_new, i+1, root->kids, NULL);
		if (root_new == NULL)
			return NULL;
		assert(root_new->length == root->length - orig_offset);
	}
	return root_new;
}

/* Create a new node tree out of an old one that ends before `offset' in the
   old one. */
static Node *chop_after(Node *root, size_t offset) {
	Node *node, *node_new, *root_new;
	Chunk *chunk, *chunk_new;
	size_t i, orig_offset;
	
	if (offset >= root->length)
		return root;

	orig_offset = offset;
	if (root->height == 2) {
		for (i = 0; ; ++i) {
			assert(i < root->kids);
			chunk = root->x.chunk[i];
			if (chunk->length > offset)
				break;
			offset -= chunk->length;
		}
		chunk_new = copy_chunk(chunk, 0, offset);
		if (chunk_new == NULL)
			return NULL;
		assert(chunk_new->length == offset);
		root_new = copy_node(root, NULL, 0, i, chunk_new);
		if (root_new == NULL)
			return NULL;
		assert(root_new->length == orig_offset);
	} else {
		for (i = 0; ; ++i) {
			assert(i < root->kids);
			node = root->x.node[i];
			if (node->length > offset)
				break;
			offset -= node->length;
		}
		node_new = chop_after(node, offset);
		if (node_new == NULL)
			return NULL;
		root_new = copy_node(root, NULL, 0, i, node_new);
		if (root_new == NULL)
			return NULL;
		assert(root_new->length == orig_offset);
	}

	assert(root_new->height == root->height);
	return root_new;
}

/* Make sure a node tree is at least of a particular height. */
static void *make_of_height(size_t wanted, void *ptr, size_t current) {
	Node *ptr_new;

	while (current < wanted) {
		ptr_new = new_empty_node();
		if (ptr_new == NULL)
			return NULL;
		ptr_new->height = current + 1;
		ptr_new->kids = 1;
		switch (current) {
		case 0:
			assert(ptr == NULL);
			ptr_new->x.chunk[0] = NULL;
			ptr_new->length = 0;
			break;
		case 1:
			ptr_new->x.chunk[0] = ptr;
			ptr_new->length = ((Chunk *) ptr)->length;
			break;
		default:
			ptr_new->x.node[0] = ptr;
			ptr_new->length = ((Node *) ptr)->length;
			break;
		}
		ptr = ptr_new;
		++current;
	}
	return ptr;
}


#if 0

static Node *make_root(Tbuf *tbuf) {
	Node *root;
	
	assert(tbuf->height > 0);
	if (tbuf->height >= 2)
		return tbuf->x.root;

	root = new_empty_node();
	if (root == NULL)
		return NULL;
	root->height = 2;
	root->length = tbuf->length;
	root->kids = 1;
	root->x.chunk[0] = tbuf->x.chunk;

	return root;
}

static Chunk *rightmost_chunk(Node *root) {
	assert(root->height >= 2);
	while (root->height > 2)
		root = root->x.node[root->kids - 1];
	return root->x.chunk[root->kids - 1];
}

static Chunk *leftmost_chunk(Node *root) {
	assert(root->height >= 2);
	while (root->height > 2)
		root = root->x.node[0];
	return root->x.chunk[0];
}

#endif

static Node *replace_rightmost_chunk(Node *root, Chunk *chunk) {
	Node *new, *new_kid;
	int i;

	new = new_empty_node();
	if (new == NULL)
		return NULL;

	new->height = root->height;
	new->kids = root->kids;

	if (root->height == 2) {
		new->length = root->length
				- root->x.chunk[root->kids-1]->length
				+ chunk->length;
		for (i = 0; i < root->kids-1; ++i)
			new->x.chunk[i] = root->x.chunk[i];
		new->x.chunk[root->kids-1] = chunk;
	} else {
		new_kid = replace_rightmost_chunk(root->x.node[root->kids-1], 
							chunk);
		new->length = root->length
				- root->x.node[root->kids-1]->length
				+ new_kid->length;
		for (i = 0; i < root->kids-1; ++i)
			new->x.node[i] = root->x.node[i];
		new->x.node[root->kids-1] = new_kid;
	}

	assert(new->kids > 0);
	return new;
}

static Tbuf *copy_tbuf(Tbuf *tbuf) {
	Tbuf *new;
	
	new = new_tbuf();
	if (new == NULL)
		return NULL;
	new->height = tbuf->height;
	new->offset = tbuf->offset;
	new->length = tbuf->length;
	new->x = tbuf->x;
	return new;
}

static Node *trim_chunk_tree(Tbuf *tbuf) {
	Node *root;
	Chunk *chunk;
	
	switch (tbuf->height) {
	case 0:
		assert(0);
		return NULL;
	
	case 1:
		root = new_empty_node();
		if (root == NULL)
			return NULL;
		chunk = tbuf->x.chunk;
		root->length = chunk->length;
		root->height = 2;
		root->kids = 1;
		root->x.chunk[0] = chunk;
		return root;
		
	default:
		root = tbuf->x.root;
		root = chop_after(root, tbuf->offset + tbuf->length);
		if (root == NULL)
			return NULL;
		return chop_before(root, tbuf->offset);
	}
	/*NOTREACHED*/
}

static void copy_kids(void **kids, int *total, void **tab, int from, int to) {
	int i;
	
	for (i = from; i <= to; ++i)
		kids[(*total)++] = tab[i];
}

static void copy_kids_to_node(Node *node, void **kids, int from, int to) {
	int i;
	Chunk *ckid;
	Node *nkid;
	
	switch (node->height) {
	case 1:
		assert(0);
		break;
	case 2:
		for (i = from; i <= to; ++i) {
			ckid = kids[i];
			node->x.chunk[node->kids++] = ckid;
			node->length += ckid->length;
		}
		break;
	default:
		for (i = from; i <= to; ++i) {
			nkid = kids[i];
			node->x.node[node->kids++] = nkid;
			node->length += nkid->length;
		}
		break;
	}
}

static void collect_right_side(void **tab, Node *root) {
	int i;
	
	for (i = root->height; i > 2; --i) {
		tab[i] = root;
		root = root->x.node[root->kids-1];
	}
	assert(root->height == 2);
	tab[1] = root->x.chunk[root->kids-1];
}

static void collect_left_side(void **tab, Node *root) {
	int i;
	
	for (i = root->height; i > 2; --i) {
		tab[i] = root;
		root = root->x.node[0];
	}
	assert(root->height == 2);
	tab[1] = root->x.chunk[0];
}

static void combine_chunks(void **p1, void **p2, Chunk *chunk1, Chunk *chunk2)
{
	size_t len1, len2, len;
	char buf[CHUNK_MAX];
	const char *cstr;
	
	assert(p1 != NULL);
	assert(p2 != NULL);
	assert(chunk1 != NULL);
	assert(chunk2 != NULL);

	len1 = chunk1->length;
	len2 = chunk2->length;
	
	if (len1 + len2 > CHUNK_MAX) {
		*p1 = chunk1;
		*p2 = chunk2;
	} else {
		memcpy(buf, chunk1->chars, len1);
		memcpy(buf + len1, chunk2->chars, len2);
		cstr = buf;
		len = len1 + len2;
		*p1 = new_chunk(&cstr, &len);
		*p2 = NULL;
		assert(((Chunk *) *p1)->length == len1 + len2);
	}
}

#define SMART_CAT_HEIGHT 100

static Node *smart_cat(Tbuf *tbuf1, Tbuf *tbuf2, Node *root1, Node *root2) {
	void *tab1[SMART_CAT_HEIGHT] = { NULL, };
	void *tab2[SMART_CAT_HEIGHT] = { NULL, };
	void *allkids[NODE_MAX * 2];
	void *p1, *p2, *q1, *q2;
	size_t i, h1, h2, h_max;
	int kids1, kids2, total_kids;

	h1 = root1->height;
	h2 = root2->height;
	
	assert(h1 > 0);
	assert(h2 > 0);
	
	if (h1 >= SMART_CAT_HEIGHT || h2 >= SMART_CAT_HEIGHT)
		return NULL;	/* stupid_cat will handle this */
	if (h1 < h2)
		h_max = h2;
	else
		h_max = h1;

	collect_right_side(tab1, root1);
	collect_left_side(tab2, root2);
	
	combine_chunks(&p1, &p2, tab1[1], tab2[1]);
	if (p1 == NULL)
		return NULL;
	
	for (i = 2; i <= h_max; ++i) {
		kids1 = (tab1[i] == NULL) ? 0 : ((Node *) tab1[i])->kids;
		kids2 = (tab2[i] == NULL) ? 0 : ((Node *) tab2[i])->kids;
		
		if (kids1 + kids2 > NODE_MAX)
			return NULL;
		
		total_kids = 0;
		copy_kids(allkids, &total_kids, tab1, 0, kids1-2);
		if (p1 != NULL)
			allkids[total_kids++] = p1;
		if (p2 != NULL)
			allkids[total_kids++] = p2;
		copy_kids(allkids, &total_kids, tab1, 1, kids2-1);
			
		if (total_kids > NODE_MAX) {
			q1 = new_empty_node();
			if (q1 == NULL)
				return NULL;
			((Node *)q1)->height = i;
			copy_kids_to_node(q1, allkids, 0, NODE_MAX-1);

			q2 = new_empty_node();
			if (q2 == NULL)
				return NULL;
			((Node *)q2)->height = i;
			copy_kids_to_node(q2, allkids, NODE_MAX, total_kids-1);
		} else {
			q1 = new_empty_node();
			if (q1 == NULL)
				return NULL;
			((Node *)q1)->height = i;
			copy_kids_to_node(q1, allkids, 0, total_kids-1);
			q2 = NULL;
		}
		
		p1 = q1;
		p2 = q2;
	}

	if (p2 != NULL)
		return NULL;
	
	assert(p1 != NULL);
	return p1;
}

static Node *stupid_cat(Tbuf *tbuf1, Tbuf *tbuf2, Node *p1, Node *p2) {
	Node *new_root;
	size_t h1, h2;
	
printf("stupid_cat: ------------------------------------------------\n");
	h1 = p1->height;
	h2 = p2->height;
	if (h1 < h2) {
		p1 = make_of_height(h2, p1, h1);
		h1 = h2;
	} else if (h2 < h2) {
		p2 = make_of_height(h1, p2, h2);
		h2 = h1;
	}
	assert(h1 > 1);
	if (p1 == NULL || p2 == NULL)
		return NULL;
	
	new_root = new_empty_node();
	new_root->height = h1 + 1;
	new_root->length = p1->length + p2->length;
	new_root->kids = 2;
	new_root->x.node[0] = p1;
	new_root->x.node[1] = p2;
	
	return new_root;
}

Tbuf *tbuf_cat(Tbuf *tbuf1, Tbuf *tbuf2) {
	Tbuf *tbuf_new;
	Node *p1, *p2, *new_root;

	if (tbuf1->length == 0)
		return copy_tbuf(tbuf2);
	if (tbuf2->length == 0)
		return copy_tbuf(tbuf1);
	
	p1 = trim_chunk_tree(tbuf1);
	p2 = trim_chunk_tree(tbuf2);
	if (p1 == NULL || p2 == NULL)
		return NULL;
	
	new_root = smart_cat(tbuf1, tbuf2, p1, p2);
	if (new_root == NULL)
		new_root = stupid_cat(tbuf1, tbuf2, p1, p2);
	if (new_root == NULL)
		return NULL;

	tbuf_new = new_tbuf();
	if (tbuf_new == NULL)
		return NULL;
	tbuf_new->height = new_root->height;
	tbuf_new->length = new_root->length;
	tbuf_new->x.root = new_root;

	return tbuf_new;
}


/*
 * tbuf_copy needs no auxiliary routines of its own.
 */

Tbuf *tbuf_copy(Tbuf *tbuf, size_t start, size_t length) {
	Tbuf *tbuf_new;
	Node *temp;

	if (start >= tbuf->length || length == 0)
		return tbuf_create("", 0);

	if (start + length > tbuf->length)
		length = tbuf->length - start;

	tbuf_new = new_tbuf();
	if (tbuf_new == NULL)
		return NULL;

	switch (tbuf->height) {
	case 0:
		tbuf_new->height = 0;
		tbuf_new->offset = 0;
		tbuf_new->length = 0;
		break;
	
	case 1:
		tbuf_new->height = 1;
		tbuf_new->offset = tbuf->offset + start;
		tbuf_new->length = length;
		tbuf_new->x.chunk = tbuf->x.chunk;
		break;
	
	default:
#if 1
		temp = chop_before(tbuf->x.root, tbuf->offset);
		temp = chop_after(temp, tbuf->length);
		while (temp->height > 2 && temp->kids == 1)
			temp = temp->x.node[0];
		tbuf_new->x.root = temp;
		tbuf_new->height = temp->height;
#else
		tbuf_new->x.root = tbuf->x.root;
		tbuf_new->height = tbuf->height;
#endif
		tbuf_new->offset = 0;
		tbuf_new->length = length;
		break;
	}

	return tbuf_new;
}


/*
 * tbuf_get_stats. Statistics gathering.
 */

static void gather_chunk_stats(struct tbuf_stat *stat, Chunk *chunk, 
int shared) {
	size_t chunk_memory;
	
	if (!chunk->visited) {
		++stat->tbuf_chunks;
		chunk_memory = sizeof(Chunk) + chunk->length;
		stat->tbuf_memory_total += chunk_memory;
		chunk->visited = 1;
	}
}

static void gather_node_stats(struct tbuf_stat *stat, Node *node, int shared) {
	size_t i;
	
	if (!node->visited) {
		++stat->tbuf_nodes;
		stat->tbuf_memory_total += sizeof(Node);
		if (node->kids < 3)
			++stat->tbuf_nodes_with_few_kids;
		if (node->height == 2) {
			for (i = 0; i < node->kids; ++i)
				gather_chunk_stats(stat, node->x.chunk[i], shared);
		} else {
			for (i = 0; i < node->kids; ++i)
				gather_node_stats(stat, node->x.node[i], shared);
		}
		node->visited = 1;
	}
}

void tbuf_get_stats(struct tbuf_stat *stat, Tbuf *tbuf) {
	stat->tbuf = tbuf;
	stat->tbuf_length = tbuf->length;
	stat->tbuf_height = tbuf->height;

	stat->tbuf_nodes = 0;
	stat->tbuf_chunks = 0;
	stat->tbuf_memory_total = sizeof(Tbuf);
	stat->tbuf_nodes_with_few_kids = 0;

	switch (tbuf->height) {
	case 0:
		break;
	
	case 1:
		tbuf->x.chunk->visited = 0;
		gather_chunk_stats(stat, tbuf->x.chunk, 0);
		break;
	
	default:
		node_zero_visited(tbuf->x.root);
		gather_node_stats(stat, tbuf->x.root, 0);
		break;
	}
}



/*
 * Return length of buffer.
 */
 
size_t tbuf_length(Tbuf *tbuf) {
	return tbuf->length;
}
