/*
 * is_clear.c -- make an iset be empty
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id$"
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
