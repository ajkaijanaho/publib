/*
 * memmem.c -- search for memory block inside another memory block
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: memmem.c,v 1.1 1994/06/20 20:29:57 liw Exp $"
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
