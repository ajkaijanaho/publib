/*
 * memrchr.c -- reverse search for character inside another memory block
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: memrchr.c,v 1.1 1994/08/28 18:13:42 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

void *memrchr(const void *v, int c, size_t size) {
	const char *p;

	assert(v != NULL);

	p = (const char *)v + size;
	while (size-- > 0)
		if (*--p == c)
			return (void *) p;
	return NULL;
}
