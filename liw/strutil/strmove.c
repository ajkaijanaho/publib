/*
 * strmove.c -- copy a string to another, possibly overlapping place
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strmove.c,v 1.1 1994/06/20 20:30:20 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

char *strmove(char *tgt, const char *src) {
	assert(tgt != NULL);
	assert(src != NULL);

	return memmove(tgt, src, strlen(src) + 1);
}
