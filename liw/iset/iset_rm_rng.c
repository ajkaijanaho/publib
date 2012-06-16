/*
 * iset_remove_range.c -- remove a range of numbers from an iset
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id: iset_rm_rng.c,v 1.1.1.1 1993/11/20 17:02:31 liw Exp $"
 */

#include <assert.h>
#include <stddef.h>	/* need NULL */
#include "publib/iset.h"

int iset_remove_range(Iset *is, long number1, long number2) {
	long i;

	assert(is != NULL);
	assert(number1 <= number2);

	for (i = number1; i <= number2; ++i)
		if (iset_remove(is, i) == -1)
			return -1;
	return 0;
}
