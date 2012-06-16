/*
 * iset_range.c -- return the range of numbers in an iset
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id: iset_range.c,v 1.1.1.1 1993/11/20 17:02:31 liw Exp $"
 */

#include <assert.h>
#include <stddef.h>	/* need NULL */
#include "publib/iset.h"

void iset_range(const Iset *is, long *lowest, long *highest) {
	assert(is != NULL);
	assert(is->lowest != NULL);
	assert(is->highest != NULL);
	if (lowest != NULL)
		*lowest = is->lowest->start;
	if (highest != NULL)
		*highest = is->highest->start;
}
