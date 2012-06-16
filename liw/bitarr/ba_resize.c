/*
 * ba_resize.c -- change size of set
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-bitarr:$Id: ba_resize.c,v 1.4 1995/05/12 14:44:06 wirzeniu Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "publib/bitarr.h"
#include "publib/errormsg.h"

int ba_resize(Bitarr *u, size_t rnglen) {
	size_t oldsize, newsize;

	assert(u != NULL);

	oldsize = ba_num2word(u->rnglen);
	newsize = ba_num2word(rnglen);
	if (dynarr_resize(&u->da, newsize) == -1)
		return -1;

	if (oldsize < newsize) {
		unsigned *w = u->da.data;
		memset(w + oldsize, 0, newsize - oldsize);
	}
	u->rnglen = rnglen;

	return 0;
}
