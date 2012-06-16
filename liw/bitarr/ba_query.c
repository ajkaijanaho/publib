/*
 * ba_query.c -- check if number belongs to set
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-bitarr:$Id: ba_query.c,v 1.2 1995/05/12 14:44:05 wirzeniu Exp $"
 */

#include <assert.h>
#include "publib/bitarr.h"

int ba_query(Bitarr *u, unsigned number) {
	unsigned *w;
	assert(u != NULL);
	if (number >= u->rnglen)
		return 0;
	assert(u->da.data != NULL);
	w = u->da.data;
	return w[ba_num2word(number)] & ba_num2mask(number);
}
