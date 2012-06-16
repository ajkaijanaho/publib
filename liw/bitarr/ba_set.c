/*
 * ba_set.c -- add number to set
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-bitarr:$Id: ba_set.c,v 1.4 1995/05/12 14:44:06 wirzeniu Exp $"
 */

#include <assert.h>
#include "publib/bitarr.h"
#include "publib/errormsg.h"

int ba_set(Bitarr *u, unsigned number) {
	unsigned *w;

	assert(u != NULL);

	if (number >= u->rnglen && ba_resize(u, number+1) == -1) {
		__publib_error("ba_resize failed");
		return -1;
	}
	w = u->da.data;
	w[ba_num2word(number)] |= ba_num2mask(number);
	return 0;
}
