/*
 * memrev.c -- reverse an array in place
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: memrev.c,v 1.1.1.1 1994/02/03 17:25:31 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

void *memrev(void *block, size_t elsize, size_t elnum) {
	char *s, *t;

	assert(block != NULL);
	assert(elsize > 0);

	for (s = block, t = s + (elnum-1)*elsize; s < t; s+=elsize, t-=elsize)
		memswap(s, t, elsize);
	return block;
}
