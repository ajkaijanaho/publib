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
 * iset_nth_rng.c -- return the ends of the nth consecutive range of values
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id: iset_nth_rng.c,v 1.1.1.1 1993/11/20 17:02:32 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/iset.h"

int iset_nth_range(const Iset *is, long n, long *lo, long *hi) {
	struct __iset_range *r;

	assert(is != NULL);
	assert(lo != NULL);
	assert(hi != NULL);
	assert(n > 0);

	for (r = is->lowest; n > 1 && r != NULL; --n, r = r->next)
		continue;
	if (r == NULL)
		return -1;
	*lo = r->start;
	*hi = r->end;
	return 0;
}
