/*
 * ba_clear.c -- remove number from set
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-bitarr:$Id: ba_clear.c,v 1.3 1995/05/12 14:43:58 wirzeniu Exp $"
 */

#include <assert.h>
#include "publib/bitarr.h"

void ba_clear(Bitarr *u, unsigned number) {
	assert(u != NULL);
	assert(u->rnglen == 0 || u->da.data != NULL);

	if (number < u->rnglen) {
		unsigned *w = u->da.data;
		w[ba_num2word(number)] &= ~ba_num2mask(number);
	}
}
