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
 * memisort.c -- sort new element in sorted array
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: memisort.c,v 1.1 1994/02/06 09:45:09 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"


void memisort(void *base, size_t nelem, size_t elsize, const void *new, 
	      int (*comp)(const void *, const void *)) {
	char *p, *top, *cbase;

	assert(base != NULL);
	assert(elsize > 0);
	assert(new != NULL);
	assert(comp != NULL);
	assert(!memoverlap(base, nelem*elsize, new, elsize));

	cbase = (char *) base;
	top = cbase + nelem*elsize;
	for (p = top; p > cbase; p -= elsize)
		if ((*comp)(new, p-elsize) >= 0)
			break;
	if (p < top)
		memmove(p+elsize, p, (size_t) (top-p));
			/* we're lost if ptrdiff_t < size_t */
	memcpy(p, new, elsize);
}
