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
