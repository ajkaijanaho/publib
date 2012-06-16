/*
 * stack_create.c -- create a new (and empty) stack
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-stack:$Id: stack_create.c,v 1.3 1994/07/16 15:29:06 liw Exp $"
 */

#include <stdlib.h>
#include "publib/stack.h"
#include "publib/errormsg.h"

Stack *stack_create(void) {
	Stack *st;

	st = malloc(sizeof(Stack));
	if (st == NULL) {
		__publib_error("malloc failed");
		return NULL;
	}

	st->data = NULL;
	st->size = NULL;
	st->allocated = 0;
	st->used = 0;
	return st;
}
