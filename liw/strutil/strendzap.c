/*
 * strendzap.c -- remove pat from end of str, if it is there
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strendzap.c,v 1.2 1994/07/22 12:22:51 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

int strendzap(char *str, const char *pat) {
	size_t len, patlen;

	assert(str != NULL);
	assert(pat != NULL);

	len = strlen(str);
	patlen = strlen(pat);

	if (patlen <= len) {
		str += len - patlen;
		if (strcmp(str, pat) == 0) {
			*str = '\0';
			return 1;
		}
	}
	return 0;
}
