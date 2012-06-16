/*
 * hash_destroy.c -- destroy a hash table
 *
 * Part of Publib, see man page for more information.
 * "@(#)publib-hash:$Id: hash_destroy.c,v 1.1.1.1 1994/07/20 21:02:54 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/hash.h"

void hash_destroy(Hashtab *ht) {
	struct __hash_node *p;
	size_t i;

	assert(ht != NULL);
	for (i = 0; i < __HASHTAB_SIZE; ++i) {
		while (ht->tab[i] != NULL) {
			p = ht->tab[i];
			ht->tab[i] = p->next;
			if (p->size > 0)
				free(p->data);
			free(p);
		}
	}
	free(ht);
}
