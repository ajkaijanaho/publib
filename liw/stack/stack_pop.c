/*
 * stack_pop.c -- remove and return topmost element of stack
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-stack:$Id$"
 */

#include <assert.h>
#include "publib/stack.h"

void *stack_pop(Stack *st) {
	void *p;

	assert(st != NULL);
	assert(st->used > 0);

	--st->used;
	p = st->data[st->used];
	st->data[st->used] = NULL;
	st->size[st->used] = 0;
	return p;
}
