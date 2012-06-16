/*
 * memdel.c -- delete bytes from beginning of memory block
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: memdel.c,v 1.1 1994/06/20 20:29:52 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"


void *memdel(void *p, size_t size, size_t n) {
	assert(p != NULL);
	assert(n <= size);
	memmove(p, (char*)p + n, size-n);
	return p;
}
