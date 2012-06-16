/*
 * ba_copy.c -- create a copy of a set
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-bitarr:$Id: ba_copy.c,v 1.3 1994/07/16 15:28:25 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "publib/bitarr.h"
#include "publib/errormsg.h"

Bitarr *ba_copy(const Bitarr *u) {
	Bitarr *uc;

	assert(u != NULL);
	assert(u->rnglen == 0 || u->da.data != NULL);

	uc = ba_create();
	if (uc == NULL)
		return NULL;
	if (dynarr_copy(&uc->da, &u->da) == -1)
		return NULL;
	uc->rnglen = u->rnglen;
	return uc;
}
