/*
 * strrev.c -- reverse a string in place
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strrev.c,v 1.3 1996/08/12 01:04:24 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

char *strrev(char *s) {
	char c, *t, *origs = s;

	assert(s != NULL);

	t = s+strlen(s);
	while (s < t) {
		c = *s;
		*s++ = *--t;
		*t = c;
	}
	return origs;
}
