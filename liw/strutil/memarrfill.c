/*
 * memarrfill.c -- copy first element of array to all elements
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: memarrfill.c,v 1.1 1994/07/12 21:04:52 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"


void *memarrfill(void *arr, size_t elsize, size_t nelem) {
	assert(arr != NULL);
	assert(elsize > 0);

	if (nelem > 0)
		memfill((char*)arr+elsize, (nelem-1)*elsize, arr, elsize);
	return arr;
}
