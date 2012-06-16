/*
 * strzap.c -- remove pat from beginning of str, if it is there
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strzap.c,v 1.2 1994/07/22 12:22:54 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

int strzap(char *str, const char *pat) {
	size_t len, patlen;

	assert(str != NULL);
	assert(pat != NULL);

	len = strlen(str);
	patlen = strlen(pat);

	if (patlen <= len && memcmp(str, pat, patlen) == 0) {
		memmove(str, str+patlen, len-patlen+1);
		return 1;
	}
	return 0;
}
