/*
 * strdel.c -- remove characters from the beginning of a string
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strdel.c,v 1.1.1.1 1994/02/03 17:25:30 liw Exp $"
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
