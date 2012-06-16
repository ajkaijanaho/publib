/*
 * strnins.c -- insert prefix of a string at the beginning of another string
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strnins.c,v 1.1 1994/07/25 23:15:39 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

char *strnins(char *tgt, const char *src, size_t n) {
	size_t srclen;

	assert(tgt != NULL);
	assert(src != NULL);
	assert(!memoverlap(tgt, strlen(tgt)+strlen(src)+1, src, strlen(src)+1));

	srclen = strlen(src);
	if (srclen > n)
		srclen = n;
	memmove(tgt + srclen, tgt, strlen(tgt) + 1);	/* +1 for '\0' */
	memcpy(tgt, src, srclen);

	return tgt;
}
