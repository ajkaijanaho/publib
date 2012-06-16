/*
 * strright.c -- return a pointer to the beginning of the rightmost n chars
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id$"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

char *strright(const char *s, size_t n) {
	size_t len;

	assert(s != NULL);
	len = strlen(s);
	if (n > len)
		n = 0;
	return (char *)s + (len - n);
}
