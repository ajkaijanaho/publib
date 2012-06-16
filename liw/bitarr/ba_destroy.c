/*
 * ba_destroy.c -- free resources allocated to a set
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-bitarr:$Id: ba_destroy.c,v 1.2 1995/05/12 14:44:01 wirzeniu Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/bitarr.h"

void ba_destroy(Bitarr *u) {
	assert(u != NULL);
	dynarr_free(&u->da);
	free(u);
}
