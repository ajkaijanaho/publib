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
 * ba_xor_ba.c -- xor a set with another set
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-bitarr:$Id: ba_xor_ba.c,v 1.3 1995/05/12 14:44:07 wirzeniu Exp $"
 */

#include <assert.h>
#include "publib/bitarr.h"

void ba_xor_ba(Bitarr *u1, const Bitarr *u2) {
	size_t i, size, u2size;
	unsigned *w1, *w2;

	assert(u1 != NULL);
	assert(u2 != NULL);
	assert(u1->rnglen == 0 || u1->da.data != NULL);
	assert(u2->rnglen == 0 || u2->da.data != NULL);

	size = ba_num2word(u1->rnglen);
	u2size = ba_num2word(u2->rnglen);
	if (u2size < size)
		size = u2size;
	w1 = u1->da.data;
	w2 = u2->da.data;
	for (i = 0; i < size; ++i)
		w1[i] ^= w2[i];
}
