/*
 * stack_destroy.c -- free resources allocated for stack
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-stack:$Id: stack_destroy.c,v 1.1.1.1 1993/11/20 17:02:51 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/stack.h"

void stack_destroy(Stack *st) {
	assert(st != NULL);
	assert(st->used <= st->allocated);
	while (st->used > 0)
		stack_pop(st);
	free(st->size);
	free(st->data);
	free(st);
}
