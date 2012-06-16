/*
 * hash.h -- hashing table routines
 *
 * Part of Publib, see man page for more information.
 * "@(#)publib-hash:$Id: hash.h,v 1.2 1995/09/23 09:11:54 liw Exp $"
 */

#ifndef __publib_hash_h_included
#define __publib_hash_h_included

#include <stddef.h>	/* need size_t */

#define __HASHTAB_SIZE	511

typedef unsigned long __hash_func(const void *);

struct __hash_node {
	void *data;
	size_t size;
	unsigned long val;
	struct __hash_node *next;
};

struct __hashtab {
	__hash_func *hash;
	int (*cmp)(const void *, const void *);
	struct __hash_node *tab[__HASHTAB_SIZE];
};

typedef struct __hashtab Hashtab;

Hashtab *hash_create(__hash_func *, int (*)(const void *, const void *));
void hash_destroy(Hashtab *);
void *hash_install(Hashtab *, const void *, size_t);
void *hash_lookup(Hashtab *, const void *);
int hash_uninstall(Hashtab *, const void *);
int hash_iter(Hashtab *, int (*)(void *, void *), void *);

#endif
