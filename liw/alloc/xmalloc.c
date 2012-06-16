/*
 * xmalloc.c -- error checking malloc
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-alloc:$Id: xmalloc.c,v 1.4 1994/12/27 12:39:53 wirzeniu Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/alloc.h"
#include "publib/errormsg.h"

void *xmalloc(size_t n) {
	char *q;

	assert(n > 0);
	q = malloc(n);
	if (q == NULL) {
		__publib_error("malloc failed");
		exit(EXIT_FAILURE);
	}
	return q;
}
