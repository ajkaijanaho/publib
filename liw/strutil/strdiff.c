/*
 * strdiff.c -- find first difference between two strings
 *
 * Part of publib.  See man page for more information.
 * "@(#)publib-strutil:$Id$"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

size_t strdiff(const char *s, const char *t) {
	size_t n;

	for (n = 0; *s != '\0' && *s++ == *t++; ++n)
		continue;
	return n;
}
