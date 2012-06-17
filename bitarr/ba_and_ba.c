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
 * ba_and_ba.c -- and a set with another set
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-bitarr:$Id: ba_and_ba.c,v 1.3 1995/05/12 14:43:58 wirzeniu Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/bitarr.h"

void ba_and_ba(Bitarr *u1, const Bitarr *u2) {
	size_t i, size, u1size;
	unsigned *w1, *w2;

	assert(u1 != NULL);
	assert(u2 != NULL);
	assert(u1->rnglen == 0 || u1->da.data != NULL);
	assert(u2->rnglen == 0 || u2->da.data != NULL);

	u1size = ba_num2word(u1->rnglen);
	size = ba_num2word(u2->rnglen);
	if (u1size < size)
		size = u1size;

	w1 = u1->da.data;
	w2 = u2->da.data;

	for (i = 0; i < size; ++i)
		*w1++ &= *w2++;
	if (size < u1size)
		memset(w1 + size, 0, u1size - size);
}
