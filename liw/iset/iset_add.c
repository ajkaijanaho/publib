/* Part of publib.

   Copyright (c) 1994-2006 Lars Wirzenius.  All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*
 * iset_add.c -- add an integer to an iset
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id: iset_add.c,v 1.5 1995/08/25 20:45:09 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/iset.h"
#include "publib/errormsg.h"


/* insert new range into a list of ranges */
static int new_range(long start, long end, struct __iset_range *before, 
		     Iset *is) {
	struct __iset_range *rnew;

	assert(before != NULL);
	assert(is != NULL);

	rnew = malloc(sizeof(struct __iset_range));
	if (rnew == NULL) {
		__publib_error("malloc failed");
		return -1;
	}

	rnew->start = start;
	rnew->end = end;

	if (before == NULL) {	/* append to list of ranges */
		rnew->next = NULL;
		rnew->prev = is->highest;
		if (is->highest != NULL)
			is->highest->next = rnew;
		is->highest = rnew;
		if (is->lowest == NULL)
			is->lowest = rnew;
	} else {		/* insert before `before' */
		rnew->next = before;
		rnew->prev = before->prev;
		before->prev = rnew;
		if (rnew->prev != NULL)
			rnew->prev->next = rnew;
		else {
			is->lowest = rnew;
			if (is->highest == NULL)
				is->highest = rnew;
		}
	}

	return 0;
}


/* see if a range is adjacent to its neighbors and join it with them if so */
static void join_ranges(Iset *is, struct __iset_range *r) {
	struct __iset_range *temp;

	assert(is != NULL);
	assert(r != NULL);

	if (r->prev != NULL && r->prev->end == r->start-1) {
		r->prev->end = r->end;
		r->prev->next = r->next;
		if (r->next != NULL)
			r->next->prev = r->prev;
		else
			is->highest = r->prev;
		temp = r;
		r = r->prev;
		free(temp);
	}

	if (r->next != NULL && r->next->start == r->end+1) {
		r->next->start = r->start;
		r->next->prev = r->prev;
		if (r->prev != NULL)
			r->prev->next = r->next;
		else
			is->lowest = r->next;
		free(r);
	}
}


int iset_add(Iset *is, long number) {
	struct __iset_range *r;

	assert(is != NULL);

	for (r = is->lowest; r != NULL; r = r->next) {
		if (r->start <= number && number <= r->end)
			return 0;	/* number already in set */
		if (number == r->start-1) {
			--r->start;
			join_ranges(is, r);
			return 0;	/* increase existing range */
		}
		if (number == r->end+1) {
			++r->end;
			join_ranges(is, r);
			return 0;	/* increase existing range */
		}
		if (number < r->start)
			return new_range(number, number, r, is);
	}
/* FIXME: the following will never work! */
	return new_range(number, number, NULL, is);
}
