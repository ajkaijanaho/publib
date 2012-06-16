/*
 * memrmem.c -- reverse search for memory block inside another memory block
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: memrmem.c,v 1.1 1994/06/20 20:30:01 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

void *memrmem(const void *v, size_t size, const void *pat, size_t patsize) {
	const char *p;

	assert(v != NULL);
	assert(pat != NULL);

	/* Special cases */
	if (size < patsize)
		return NULL;
	if (patsize == 0)
		return (void *) v;

	/* xxx - this will make p < v if pattern not found; potential
	   portability problem */
	for (p = v, p += size-patsize; size >= patsize; --p, --size)
		if (memcmp(p, pat, patsize) == 0)
			return (void *) p;
	return NULL;
}
