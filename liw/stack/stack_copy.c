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
