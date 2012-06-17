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
 * memshuffle.c -- make an array be in random order
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: memshuffle.c,v 1.3 1994/06/20 20:30:02 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "publib/strutil.h"


void *memshuffle(void *base, size_t elsize, size_t nelem) {
	size_t i;
	char *p;

	assert(base != NULL);
	assert(elsize > 0);

	for (p = base; nelem > 0; --nelem, p += elsize) {
		i = rand() / (RAND_MAX / nelem + 1);
		if (i > 0)
			memswap(p, p + i*elsize, elsize);
	}
	return base;
}
