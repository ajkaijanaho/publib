/*
 * iset_add_range.c -- add a range of integers to an iset
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id$"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/iset.h"

int iset_add_range(Iset *is, long number1, long number2) {
	long i;

	assert(is != NULL);
	assert(number1 <= number2);

	for (i = number1; i <= number2; ++i)
		if (iset_add(is, i) == -1)
			return -1;
	return 0;
}
