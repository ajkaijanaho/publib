/*
 * strinit.c -- initialize a string to a given length with a given character
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strinit.c,v 1.1 1994/06/20 20:30:17 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

char *strinit(char *s, int c, size_t len) {
	assert(s != NULL);

	memset(s, c, len);
	s[len] = '\0';
	return s;
}
