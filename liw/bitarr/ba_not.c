/*
 * ba_not.c -- complement a set
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-bitarr:$Id: ba_not.c,v 1.2 1995/05/12 14:44:03 wirzeniu Exp $"
 */

#include <assert.h>
#include "publib/bitarr.h"

void ba_not(Bitarr *u) {
	size_t i, size;
	unsigned *w;

	assert(u != NULL);
	assert(u->rnglen == 0 || u->da.data != NULL);

	size = ba_num2word(u->rnglen);
	w = u->da.data;
	for (i = 0; i < size; ++i)
		w[i] = ~w[i];
}
