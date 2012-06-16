/*
 * ba_clear_all.c -- make set empty
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-bitarr:$Id: ba_clear_all.c,v 1.2 1995/05/12 14:43:59 wirzeniu Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/bitarr.h"

void ba_clear_all(Bitarr *u) {
	assert(u != NULL);
	assert(u->rnglen == 0 || u->da.data != NULL);

	if (u->da.alloc > 0)
		memset(u->da.data, 0, sizeof(unsigned)*u->da.alloc);
}
