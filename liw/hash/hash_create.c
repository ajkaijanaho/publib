/*
 * hash_create.c -- create a hash table
 *
 * Part of Publib, see man page for more information.
 * "@(#)publib-hash:$Id: hash_create.c,v 1.1.1.1 1994/07/20 21:02:54 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/hash.h"
#include "publib/errormsg.h"

Hashtab *hash_create(__hash_func *h, int (*cmp)(const void *, const void *)) {
	Hashtab *ht;
	size_t i;

	assert(h != NULL);
	assert(cmp != NULL);

	ht = malloc(sizeof(Hashtab));
	if (ht == NULL) {
		__publib_error("malloc failed");
		return NULL;
	}

	ht->hash = h;
	ht->cmp = cmp;
	for (i = 0; i < __HASHTAB_SIZE; ++i)
		ht->tab[i] = NULL;
	return ht;
}
