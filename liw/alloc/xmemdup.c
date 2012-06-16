/*
 * xmemdup.c -- error checking memdup
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-alloc:$Id: xmemdup.c,v 1.1.1.1 1993/11/20 17:01:29 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/alloc.h"
#include "publib/errormsg.h"

void *xmemdup(const void *p, size_t n) {
	char *q;

	assert(p != NULL);
	assert(n > 0);
	q = memdup(p, n);
	if (q == NULL) {
		__publib_error("memdup failed");
		exit(EXIT_FAILURE);
	}
	return q;
}
