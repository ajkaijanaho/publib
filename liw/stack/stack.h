/*
 * stack.h -- stack routines
 *
 * Part of publib
 * "@(#)publib-stack:$Id: stack.h,v 1.1.1.1 1993/11/20 17:02:51 liw Exp $"
 */

#ifndef __publib_stack_h
#define __publib_stack_h

#include <stddef.h>	/* need size_t */

struct __stack {
	void **data;
	size_t *size;
	size_t allocated, used;
};
typedef struct __stack Stack;

Stack *stack_create(void);
void stack_destroy(Stack *);
void *stack_pop(Stack *);
int stack_is_empty(Stack *);
Stack *stack_copy(Stack *);
int stack_push(Stack *, void *__data, size_t __bytes);

#endif
