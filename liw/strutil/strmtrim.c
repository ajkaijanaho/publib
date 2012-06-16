/*
 * strmtrim.c -- replace multiple white spaces with single blanks within string
 *
 * Part of publib.  See man page for more information.
 * "@(#)publib-strutil:$Id: strmtrim.c,v 1.1 1994/06/20 20:30:22 liw Exp $"
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
