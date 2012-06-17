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
