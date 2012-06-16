/*
 * hash_lookup.c -- find a datum in a hash table
 *
 * Part of Publib, see man page for more information.
 * "@(#)publib-hash:$Id: hash_lookup.c,v 1.2 1995/09/23 09:11:55 liw Exp $"
 */

#include <assert.h>
#include "publib/hash.h"

void *hash_lookup(Hashtab *ht, const void *data) {
	struct __hash_node *p;
	unsigned long val;

	assert(ht != NULL);

	val = ht->hash(data);
	for (p = ht->tab[val % __HASHTAB_SIZE]; p != NULL; p = p->next)
		if (val == p->val && ht->cmp(data, p->data) == 0)
			return p->data;
	return NULL;
}
