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
