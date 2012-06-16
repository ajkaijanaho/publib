/*
 * iset_contains.c -- check whether an iset contains a number
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id: iset_contains.c,v 1.1.1.1 1993/11/20 17:02:31 liw Exp $"
 */

#include <assert.h>
#include <stddef.h>	/* need NULL */
#include "publib/iset.h"

int iset_contains(const Iset *is, long number) {
	struct __iset_range *r;

	assert(is != NULL);

	for (r = is->lowest; r != NULL && r->end < number; r = r->next)
		continue;
	return (r != NULL && r->start <= number && number <= r->end);
}
