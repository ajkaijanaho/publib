/*
 * iset_diff.c -- take set difference
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id: iset_diff.c,v 1.1.1.1 1993/11/20 17:02:31 liw Exp $"
 */

#include <assert.h>
#include <stddef.h>	/* need NULL */
#include "publib/iset.h"

int iset_diff(Iset *is1, const Iset *is2) {
	struct __iset_range *r;

	assert(is1 != NULL);
	assert(is2 != NULL);

	for (r = is2->lowest; r != NULL; r = r->next)
		if (iset_remove_range(is1, r->start, r->end) == -1)
			return -1;
	return 0;
}
