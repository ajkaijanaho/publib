/*
 * strrstr.c -- find last occurence of string in another string
 *
 * Part of publib.  See man page for more information.
 * "@(#)publib-strutil:$Id: strrstr.c,v 1.1.1.1 1994/02/03 17:25:29 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

char *strrstr(const char *str, const char *pat) {
	size_t len, patlen;
	const char *p;

	assert(str != NULL);
	assert(pat != NULL);

	len = strlen(str);
	patlen = strlen(pat);

	if (patlen > len)
		return NULL;
	for (p = str + (len - patlen); p > str; --p)
		if (*p == *pat && strncmp(p, pat, patlen) == 0)
			return (char *) p;
	return NULL;
}
