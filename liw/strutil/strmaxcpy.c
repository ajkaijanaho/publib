/*
 * strmaxcpy.c -- copy at most a given number of characters of string
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strmaxcpy.c,v 1.3 1994/07/31 23:30:44 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

char *strmaxcpy(char *tgt, const char *src, size_t n) {
	assert(tgt != NULL);
	assert(src != NULL);
	assert(!memoverlap(tgt, n+1, src, strlen(src)+1));

	*tgt = '\0';
	strncat(tgt, src, n);

	return tgt;
}
