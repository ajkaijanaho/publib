/*
 * strdiff.c -- find first difference between two strings
 *
 * Part of publib.  See man page for more information.
 * "@(#)publib-strutil:$Id: strdiff.c,v 1.1 1995/08/14 22:05:35 liw Exp $"
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
