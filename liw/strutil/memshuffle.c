/*
 * memshuffle.c -- make an array be in random order
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: memshuffle.c,v 1.3 1994/06/20 20:30:02 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "publib/strutil.h"


void *memshuffle(void *base, size_t elsize, size_t nelem) {
	size_t i;
	char *p;

	assert(base != NULL);
	assert(elsize > 0);

	for (p = base; nelem > 0; --nelem, p += elsize) {
		i = rand() / (RAND_MAX / nelem + 1);
		if (i > 0)
			memswap(p, p + i*elsize, elsize);
	}
	return base;
}
