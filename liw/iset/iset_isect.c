/*
 * iset_iset.c -- remove elements from set that are not also in another set
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id: iset_isect.c,v 1.1.1.1 1993/11/20 17:02:32 liw Exp $"
 */

#include <assert.h>
#include <stddef.h>	/* need NULL */
#include "publib/iset.h"

int iset_isect(Iset *is1, const Iset *is2) {
	struct __iset_range *r;
	long i;

	assert(is1 != NULL);
	assert(is2 != NULL);

	for (r = is1->lowest; r != NULL; r = r->next)
		for (i = r->start; i <= r->end; ++i)
			if (!iset_contains(is2, i))
				if (iset_remove(is1, i) == -1)
					return -1;
	return 0;
}
