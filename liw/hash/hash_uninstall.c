/*
 * hash_uninstall.c -- remove a datum from a hash table
 *
 * Part of Publib, see man page for more information.
 * "@(#)publib-hash:$Id: hash_uninstall.c,v 1.2 1995/09/23 09:11:55 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/hash.h"

int hash_uninstall(Hashtab *ht, const void *data) {
	struct __hash_node *p, *prev;
	unsigned long val;
	size_t i;

	assert(ht != NULL);
	assert(data != NULL);

	val = ht->hash(data);
	i = val % __HASHTAB_SIZE;
	for (prev = NULL, p = ht->tab[i]; p != NULL; prev = p, p = p->next) {
		if (val == p->val && ht->cmp(data, p->data) == 0) {
			if (prev == NULL)
				ht->tab[i] = p->next;
			else
				prev->next = p->next;
			if (p->size > 0)
				free(p->data);
			free(p);
			return 0;
		}
	}
	return -1;
}
