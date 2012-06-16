/*
 * hash_install.c -- add a new data value in a hash table
 *
 * Part of Publib, see man page for more information.
 * "@(#)publib-hash:$Id: hash_install.c,v 1.3 1995/11/26 10:13:09 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "publib/hash.h"
#include "publib/errormsg.h"

void *hash_install(Hashtab *ht, const void *data, size_t size) {
	struct __hash_node *p;
	unsigned long val;
	size_t i;

	assert(ht != NULL);
	assert(data != NULL);

	val = ht->hash(data);
	i = val % __HASHTAB_SIZE;
	for (p = ht->tab[i]; p != NULL; p = p->next)
		if (val == p->val && ht->cmp(data, p->data) == 0)
			return p->data;
	p = malloc(sizeof(struct __hash_node));
	if (p == NULL) {
		__publib_error("malloc failed");
		return NULL;
	}
	if (size == 0)
		p->data = (void *) data;
	else if ((p->data = malloc(size)) == NULL) {
		__publib_error("malloc failed");
		free(p);
		return NULL;
	} else
		memcpy(p->data, data, size);
	p->size = size;
	p->val = val;
	p->next = ht->tab[i];
	ht->tab[i] = p;
	return p->data;
}
