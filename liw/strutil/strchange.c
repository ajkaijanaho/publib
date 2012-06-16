/*
 * strchange.c -- change beginning of string to another string
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strchange.c,v 1.1 1994/06/20 20:30:08 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

char *strchange(char *str, size_t changelen, const char *new, size_t newlen) {
	size_t n;

	assert(str != NULL);
	assert(new != NULL);
	assert(!stroverlap(str, new));

	n = strlen(str);
	if (n < changelen)
		changelen = n;
	n = strlen(new);
	if (n < newlen)
		newlen = n;
	memmove(str+newlen, str+changelen, strlen(str+changelen)+1);
	memcpy(str, new, newlen);

	return str;
}
