/*
 * strtrim.c -- remove leading and trailing blanks from string
 *
 * Part of publib.  See man page for more information.
 * "@(#)publib-strutil:$Id$"
 */

#include <assert.h>
#include "publib/strutil.h"

char *strtrim(char *s) {
	assert(s != NULL);
	strrtrim(s);
	strltrim(s);
	return s;
}
