/*
 * memdup.c -- duplicate memory block
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-alloc:$Id: memdup.c,v 1.2 1995/11/26 10:13:06 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "publib/alloc.h"
#include "publib/errormsg.h"

void *memdup(const void *p, size_t n) {
	void *q;

	assert(p != NULL);
	assert(n > 0);

	q = malloc(n);
	if (q == NULL)
		__publib_error("malloc failed");
	else
		memcpy(q, p, n);
	return q;
}
