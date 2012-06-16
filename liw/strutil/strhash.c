/*
 * strhash.c -- compute a hash value for a string
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strhash.c,v 1.1 1994/06/20 20:30:15 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

unsigned long strhash(const char *s) {
	unsigned long h;

	assert(s != NULL);
	for (h = 0; *s != '\0'; ++s)
		h = (h * 33) | *s;
	return h;
}
