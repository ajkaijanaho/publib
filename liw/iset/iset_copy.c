/*
 * iset_copy.c -- create a copy of an iset
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id: iset_copy.c,v 1.3 1994/07/16 15:28:52 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "publib/iset.h"
#include "publib/errormsg.h"

Iset *iset_copy(const Iset *is) {
	Iset *isnew;
	struct __iset_range *r, *rnew;

	assert(is != NULL);

	isnew = iset_create();
	if (isnew == NULL)
		return NULL;
	for (r = is->lowest; r != NULL; r = r->next) {
		rnew = malloc(sizeof(struct __iset_range));
		if (rnew == NULL) {
			__publib_error("malloc failed");
			iset_destroy(isnew);
			return NULL;
		}
		rnew->start = r->start;
		rnew->end = r->end;
		rnew->prev = isnew->highest;
		rnew->next = NULL;
		isnew->highest = rnew;
		if (isnew->lowest == NULL)
			isnew->lowest = rnew;
	}

	return isnew;
}
