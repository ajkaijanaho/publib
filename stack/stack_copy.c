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
 * stack_copy.c -- make a copy of a stack and pushed data
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-stack:$Id: stack_copy.c,v 1.3 1994/07/16 15:29:05 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "publib/stack.h"
#include "publib/errormsg.h"

Stack *stack_copy(Stack *st) {
	Stack *cp;
	size_t i;

	assert(st != NULL);

	cp = malloc(sizeof(Stack));
	if (cp == NULL) {
		__publib_error("malloc failed");
		return NULL;
	}

	cp->data = NULL;	/* initializations needed for error return */
	cp->size = NULL;

	cp->allocated = cp->used = st->used;

	if (cp->used == 0)
		return cp;

	cp->data = malloc(st->used * sizeof(void *));
	if (cp->data == NULL)
		goto error;

	cp->size = malloc(st->used * sizeof(size_t));
	if (cp->size == NULL)
		goto error;

	for (i = 0; i < st->used; ++i) {
		if (st->size[i] == 0) {
			cp->data[i] = st->data[i];
			cp->size[i] = st->size[i];
		} else {
			cp->data[i] = malloc(st->size[i]);
			if (cp->data[i] == NULL)
				goto error;
			memcpy(cp->data[i], st->data[i], st->size[i]);
			cp->size[i] = st->size[i];
		}
	}

	return 0;

 error:
	__publib_error("malloc failed");
	free(cp->data);
	free(cp->size);
	free(cp);
	return NULL;
}
