/*
 * strset.c -- set all characters in a string to a given character
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strset.c,v 1.1 1994/06/20 20:30:36 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

char *strset(char *str, int c) {
	char *s;
	assert(str != NULL);

	for (s = str; *s != '\0'; ++s)
		*s = c;
	return str;
}
