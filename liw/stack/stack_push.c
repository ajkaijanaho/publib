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
