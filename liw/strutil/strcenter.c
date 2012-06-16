/*
 * strcenter.c -- center a string within given margins
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strcenter.c,v 1.1 1994/10/06 11:10:34 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

char *strcenter(char *s, size_t w) {
	for (t = s; isspace(*t); ++t)
		continue;
	strrtrim(t);
	len = strlen(t);
	if (len >= w)
		strmove(s, t);
	else {
		strmove(s+(w-len)/2, t);
		memset(s, ' ', (w-len)/2);
	}
	return s;
}
