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
 * memswap.c -- swap contents of two memory blocks
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: memswap.c,v 1.2 1994/02/05 17:08:43 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

/* The size of the auxiliary array that is used while swapping */
#define N	1024

void memswap(void *block1, void *block2, size_t n) {
	char *s, *t, buf[N];

	assert(block1 != NULL);
	assert(block2 != NULL);
	assert(!memoverlap(block1, n, block2, n));

	s = block1;
	t = block2;
	while (n > N) {
		memcpy(buf, s, N);
		memcpy(s, t, N);
		memcpy(t, buf, N);
		s += N;
		t += N;
		n -= N;
	}
	memcpy(buf, s, n);
	memcpy(s, t, n);
	memcpy(t, buf, n);
}
