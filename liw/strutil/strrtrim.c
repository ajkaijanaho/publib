/*
 * strrtrim.c -- remove trailing whitespace from a string
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strrtrim.c,v 1.2 1994/06/20 20:30:34 liw Exp $"
 */

#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "publib/strutil.h"

char *strrtrim(char *s) {
	char *t, *tt;

	assert(s != NULL);

	for (tt = t = s; *t != '\0'; ++t)
		if (!isspace(*t))
			tt = t+1;
	*tt = '\0';

	return s;
}
