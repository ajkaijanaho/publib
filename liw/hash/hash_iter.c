/*
 * hash_iter.c -- iterate through all datums in a hash table
 *
 * Part of Publib, see man page for more information.
 * "@(#)publib-hash:$Id: hash_iter.c,v 1.1.1.1 1994/07/20 21:02:54 liw Exp $"
 */

#include <assert.h>
#include "publib/hash.h"

int hash_iter(Hashtab *ht, int (*doit)(void *, void *), void *param) {
	struct __hash_node *p;
	size_t i;

	assert(ht != NULL);
	assert(doit != NULL);

	for (i = 0; i < __HASHTAB_SIZE; ++i)
		for (p = ht->tab[i]; p != NULL; p = p->next)
			switch (doit(p->data, param)) {
			case 0: return 0;
			case -1: return -1;
			}
	return 1;
}
