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
 * stack_push.c -- push element on top of stack
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-stack:$Id: stack_push.c,v 1.3 1994/07/16 15:29:08 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "publib/stack.h"
#include "publib/errormsg.h"

int stack_push(Stack *st, void *data, size_t bytes) {
	void *p;
	size_t n;

	assert(st != NULL);
	assert(st->used <= st->allocated);
	assert(data != NULL);

	if (st->used == st->allocated) {
		n = st->allocated + 64;

		p = realloc(st->data, n * sizeof(void *));
		if (p == NULL) {
			__publib_error("realloc failed");
			return -1;
		}
		st->data = p;

		p = realloc(st->size, n * sizeof(size_t));
		if (p == NULL) {
			__publib_error("realloc failed");
			return -1;
		}
		st->size = p;

		while (st->allocated < n) {
			st->data[st->allocated] = NULL;
			st->size[st->allocated] = 0;
			++st->allocated;
		}
	}

	if (bytes == 0) {
		st->data[st->used] = data;
		st->size[st->used] = 0;
	} else {
		p = malloc(bytes);
		if (p == NULL) {
			__publib_error("malloc failed");
			free(p);
			return -1;
		}
		memcpy(p, data, bytes);
		st->data[st->used] = p;
		st->size[st->used] = bytes;
	}

	++st->used;

	return 0;
}
