/*
 * memisort.c -- sort new element in sorted array
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: memisort.c,v 1.1 1994/02/06 09:45:09 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"


void memisort(void *base, size_t nelem, size_t elsize, const void *new, 
	      int (*comp)(const void *, const void *)) {
	char *p, *top, *cbase;

	assert(base != NULL);
	assert(elsize > 0);
	assert(new != NULL);
	assert(comp != NULL);
	assert(!memoverlap(base, nelem*elsize, new, elsize));

	cbase = (char *) base;
	top = cbase + nelem*elsize;
	for (p = top; p > cbase; p -= elsize)
		if ((*comp)(new, p-elsize) >= 0)
			break;
	if (p < top)
		memmove(p+elsize, p, (size_t) (top-p));
			/* we're lost if ptrdiff_t < size_t */
	memcpy(p, new, elsize);
}
