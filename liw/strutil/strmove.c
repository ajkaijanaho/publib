/*
 * strmove.c -- copy a string to another, possibly overlapping place
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id$"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

char *strmove(char *tgt, const char *src) {
	assert(tgt != NULL);
	assert(src != NULL);

	return memmove(tgt, src, strlen(src) + 1);
}
