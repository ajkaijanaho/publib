/*
 * tbuf.h -- editor buffer implemented with a tree
 *
 * Part of Publib, See manual pages publib(3) and tbuf(3) for more information.
 * "@(#)publib-tbuf:$Id: tbuf.h,v 1.4 1998/12/23 18:03:21 liw Exp $"
 */

#ifndef __publib_tbuf_h_included
#define __publib_tbuf_h_included

#include <stddef.h>

typedef struct tbuf Tbuf;

Tbuf *tbuf_create(const char *, size_t);
void tbuf_destroy(Tbuf *);
Tbuf *tbuf_copy(Tbuf *, size_t, size_t);
Tbuf *tbuf_cat(Tbuf *, Tbuf *);
size_t tbuf_length(Tbuf *);
void tbuf_chars(char *, Tbuf *, size_t, size_t);

struct tbuf_stat {
	Tbuf *tbuf;
	size_t tbuf_length;
	size_t tbuf_height;
	size_t tbuf_nodes;
	size_t tbuf_chunks;
	size_t tbuf_memory_total;
	size_t tbuf_nodes_with_few_kids;
};
void tbuf_get_stats(struct tbuf_stat *, Tbuf *);

#endif
