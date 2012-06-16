/*
 * iset_nth_rng.c -- return the ends of the nth consecutive range of values
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id: iset_nth_rng.c,v 1.1.1.1 1993/11/20 17:02:32 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/iset.h"

int iset_nth_range(const Iset *is, long n, long *lo, long *hi) {
	struct __iset_range *r;

	assert(is != NULL);
	assert(lo != NULL);
	assert(hi != NULL);
	assert(n > 0);

	for (r = is->lowest; n > 1 && r != NULL; --n, r = r->next)
		continue;
	if (r == NULL)
		return -1;
	*lo = r->start;
	*hi = r->end;
	return 0;
}
