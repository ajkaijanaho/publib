/*
 * memmem.c -- search for memory block inside another memory block
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id$"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

void *memmem(const void *v, size_t size, const void *pat, size_t patsize) {
	const char *p;

	assert(v != NULL);
	assert(pat != NULL);

	for (p = v; size >= patsize; ++p, --size)
		if (memcmp(p, pat, patsize) == 0)
			return (void*) p;
	return NULL;
}
