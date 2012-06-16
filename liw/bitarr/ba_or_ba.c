/*
 * ba_or_ba.c -- or a set with another set
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-bitarr:$Id: ba_or_ba.c,v 1.4 1995/05/12 14:44:04 wirzeniu Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "publib/bitarr.h"
#include "publib/errormsg.h"

int ba_or_ba(Bitarr *u1, const Bitarr *u2) {
	size_t i, size;
	unsigned *w1, *w2;

	assert(u1 != NULL);
	assert(u2 != NULL);
	assert(u1->rnglen == 0 || u1->da.data != NULL);
	assert(u2->rnglen == 0 || u2->da.data != NULL);

	if (u1->rnglen < u2->rnglen && ba_resize(u1, u2->rnglen) == -1) {
		__publib_error("ba_resize failed");
		return -1;
	}

	assert(u1->rnglen >= u2->rnglen);
	size = ba_num2word(u2->rnglen);
	w1 = u1->da.data;
	w2 = u2->da.data;
	for (i = 0; i < size; ++i)
		w1[i] |= w2[i];

	return 0;
}
