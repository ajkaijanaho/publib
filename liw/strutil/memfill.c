/*
 * memfill.c -- fill memory area with pattern
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: memfill.c,v 1.2 1994/02/05 17:08:42 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"


void *memfill(void *buf, size_t bufsize, const void *pat, size_t patsize) {
	char *p;

	assert(buf != NULL);
	assert(bufsize > 0);
	assert(pat != NULL);
	assert(patsize > 0);
	assert(!memoverlap(buf, bufsize, pat, patsize));

	for (p = buf; bufsize > patsize; p += patsize, bufsize -= patsize)
		memcpy(p, pat, patsize);
	memcpy(p, pat, bufsize);
	return buf;
}
