/*
 * strright.c -- return a pointer to the beginning of the rightmost n chars
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strright.c,v 1.1 1994/06/20 20:30:32 liw Exp $"
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
