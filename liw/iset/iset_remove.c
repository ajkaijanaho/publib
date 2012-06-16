/*
 * iset_remove.c -- remove a number from an iset
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id: iset_remove.c,v 1.3 1994/07/16 15:28:53 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/iset.h"
#include "publib/errormsg.h"

int iset_remove(Iset *is, long number) {
	struct __iset_range *r, *rnew;

	assert(is != NULL);

	for (r = is->lowest; r != NULL && r->end < number; r = r->next)
		continue;

	if (r == NULL || r->start > number)
		return 0;	/* number not in iset, nothing to do */

	assert(r->start <= number && number <= r->end);

	if (r->start == number && r->end == number) {
		if (r->prev == NULL)
			is->lowest = r->next;
		else
			r->prev->next = r->next;
		if (r->next == NULL)
			is->highest = r->prev;
		else
			r->next->prev = r->prev;
		free(r);
		return 0;
	}

	if (r->start == number) {
		++r->start;
		return 0;
	}

	if (r->end == number) {
		--r->end;
		return 0;
	}

	/* split range into two */

	rnew = malloc(sizeof(struct __iset_range));
	if (rnew == NULL) {
		__publib_error("malloc failed");
		return -1;
	}
	rnew->start = number+1;
	rnew->end = r->end;
	r->end = number-1;

	assert(is->lowest != NULL);

	rnew->next = r->next;
	rnew->prev = r;
	r->next = rnew;
	if (rnew->next != NULL)
		rnew->next->prev = rnew;

	return 0;
}
