/*
 * strdel.c -- remove characters from the beginning of a string
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id$"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

char *strdel(char *s, size_t n) {
	size_t len;

	assert(s != NULL);

	len = strlen(s);
	if (n > len)
		n = len;
	memmove(s, s+n, len+1 - n);
	return s;
}
