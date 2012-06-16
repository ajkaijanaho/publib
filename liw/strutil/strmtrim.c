/*
 * strmtrim.c -- replace multiple white spaces with single blanks within string
 *
 * Part of publib.  See man page for more information.
 * "@(#)publib-strutil:$Id: strtrim.c,v 1.1.1.1 1994/02/03 17:25:29 liw Exp $"
 */

#include <assert.h>
#include <ctype.h>
#include "publib/strutil.h"

char *strmtrim(char *str) {
	char *s, *t, *u;

	assert(str != NULL);

	for (s = str; isspace(*s); ++s)
		continue;

	t = s;
	for (;;) {
		if (!isspace(*t)) {
			*s = *t;
			if (*t == '\0')
				break;
			++s;
			++t;
		} else {
			u = t;
			while (isspace(*++t))
				continue;
			if (*t == '\0') {
				while ((*s++ = *u++) != '\0')
					continue;
				break;
			}
			*s++ = ' ';
		}
	}

	return str;
}
