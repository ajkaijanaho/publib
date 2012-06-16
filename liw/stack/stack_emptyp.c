/*
 * stack_is_empty.c -- test if stack is empty
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-stack:$Id: stack_emptyp.c,v 1.1.1.1 1993/11/20 17:02:51 liw Exp $"
 */

#include <assert.h>
#include "publib/stack.h"

int stack_is_empty(Stack *st) {
	assert(st != NULL);
	return st->used == 0;
}
