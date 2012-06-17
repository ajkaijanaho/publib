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
