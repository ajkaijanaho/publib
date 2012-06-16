/*
 * xrealloc.c -- error checking realloc
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-alloc:$Id: xrealloc.c,v 1.3 1994/07/16 12:06:38 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/alloc.h"
#include "publib/errormsg.h"

void *xrealloc(void *p, size_t n) {
	char *q;

	assert(n > 0);

	if (p == NULL)
		q = malloc(n);
	else
		q = realloc(p, n);
	if (q == NULL)
		__publib_error("malloc or realloc failed");
	return q;
}
