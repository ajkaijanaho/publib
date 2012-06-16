/*
 * strltrim.c -- remove leading whitespace from string
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strltrim.c,v 1.1.1.1 1994/02/03 17:25:29 liw Exp $"
 */

#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "publib/strutil.h"

char *strltrim(char *s) {
	char *t;

	assert(s != NULL);
	for (t = s; isspace(*t); ++t)
		continue;
	memmove(s, t, strlen(t)+1);	/* +1 so that '\0' is moved too */
	return s;
}
