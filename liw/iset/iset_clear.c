/*
 * is_clear.c -- make an iset be empty
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id: iset_clear.c,v 1.1.1.1 1993/11/20 17:02:31 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/iset.h"

void iset_clear(Iset *is) {
	struct __iset_range *r;

	assert(is != NULL);
	while (is->lowest != NULL) {
		r = is->lowest;
		is->lowest = is->lowest->next;
		free(r);
	}
	is->highest = NULL;
}
