/*
 * memins.c -- insert bytes at beginning of memory block
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: memins.c,v 1.1 1994/02/05 17:09:24 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"


void *memins(void *tgt, size_t tgtsize, const void *src, size_t srcsize) {
	assert(tgt != NULL);
	assert(src != NULL);
	assert(!memoverlap(tgt, tgtsize, src, srcsize));

	if (tgtsize > 0)
		memmove((char *)tgt + srcsize, tgt, tgtsize);
	if (srcsize > 0)
		memcpy(tgt, src, srcsize);
	return tgt;
}
