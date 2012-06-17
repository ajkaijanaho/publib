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
#ifndef __publib_bitarr_h
#define __publib_bitarr_h

#include <stddef.h>	/* need size_t */
#include <limits.h>	/* need CHAR_BIT */
#include <publib/alloc.h>	/* need dynarr */

typedef struct {
	struct dynarr da;
	size_t rnglen;
} Bitarr;

/* How many bits are there in an unsigned int? */
#define ba_bits		(sizeof(unsigned)*CHAR_BIT)

/* Compute word a given bit is stored in */
#define ba_num2word(i)	((i) / ba_bits)

/* Create bitmask for one word, given index into bit array */
#define ba_num2mask(i)	(1 << ((i) % ba_bits))

Bitarr *ba_create(void);
void ba_destroy(Bitarr *__ba);
Bitarr *ba_copy(const Bitarr *__ba);
int ba_resize(Bitarr *__ba, size_t __max_number);

int ba_set(Bitarr *__ba, unsigned __number);		/* ba[number] = 1 */
void ba_clear(Bitarr *__ba, unsigned __number);		/* ba[number] = 0 */
void ba_clear_all(Bitarr *__ba);			/* ba[*] = 0 */
int ba_query(Bitarr *__ba, unsigned __number);		/* ba[number] */
void ba_and_ba(Bitarr *__ba1, const Bitarr *__ba2);	/* ba1 &= ba2 */
int ba_or_ba(Bitarr *__ba1, const Bitarr *__ba2);	/* ba1 |= ba2 */
void ba_xor_ba(Bitarr *__ba1, const Bitarr *__ba2);	/* ba1 ^= ba2 */
void ba_or_not_ba(Bitarr *__ba1, const Bitarr *__ba2);	/* ba1 |= ~ba2 */
void ba_not(Bitarr *__ba);				/* ba = ~ba */

#endif
