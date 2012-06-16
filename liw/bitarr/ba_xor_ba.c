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
