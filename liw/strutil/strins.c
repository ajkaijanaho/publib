/*
 * strins.c -- insert a string at the beginning of another string
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strins.c,v 1.2 1994/02/05 17:08:44 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

char *strins(char *tgt, const char *src) {
	size_t srclen;

	assert(tgt != NULL);
	assert(src != NULL);
	assert(!memoverlap(tgt, strlen(tgt)+strlen(src)+1, src, strlen(src)+1));

	srclen = strlen(src);
	memmove(tgt + srclen, tgt, strlen(tgt) + 1);	/* +1 for '\0' */
	memcpy(tgt, src, srclen);

	return tgt;
}
