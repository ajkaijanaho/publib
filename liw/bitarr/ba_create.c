/*
 * ba_create.c -- create an empty set
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-bitarr:$Id: ba_create.c,v 1.4 1995/03/10 10:59:30 wirzeniu Exp $"
 */

#include <stdlib.h>
#include "publib/bitarr.h"
#include "publib/errormsg.h"

Bitarr *ba_create(void) {
	Bitarr *u;

	u = malloc(sizeof(Bitarr));
	if (u == NULL) {
		__publib_error("malloc failed");
		return NULL;
	}

	dynarr_init(&u->da, sizeof(unsigned));
	u->rnglen = 0;

	return u;
}
