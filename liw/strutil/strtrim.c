/*
 * strtrim.c -- remove leading and trailing blanks from string
 *
 * Part of publib.  See man page for more information.
 * "@(#)publib-strutil:$Id: strtrim.c,v 1.1.1.1 1994/02/03 17:25:29 liw Exp $"
 */

#include <assert.h>
#include "publib/strutil.h"

char *strtrim(char *s) {
	assert(s != NULL);
	strrtrim(s);
	strltrim(s);
	return s;
}
