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
