/*
 * stack_is_empty.c -- test if stack is empty
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-stack:$Id$"
 */

#include <assert.h>
#include "publib/stack.h"

int stack_is_empty(Stack *st) {
	assert(st != NULL);
	return st->used == 0;
}
