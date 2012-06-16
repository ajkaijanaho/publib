/*
 * strsub.c -- substitute first occurence of pattern with another string
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strsub.c,v 1.1.1.1 1994/02/03 17:25:30 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

char *strsub(char *str, const char *pat, const char *sub) {
	size_t lenpat, lensub, lenstr;

	assert(str != NULL);
	assert(pat != NULL);
	assert(*pat != '\0');
	assert(sub != NULL);

	str = strstr(str, pat);
	if (str == NULL)
		return NULL;

	lenstr = strlen(str);
	lenpat = strlen(pat);
	lensub = strlen(sub);

	/* make room for substituted string, or remove slack after it */
	if (lensub != lenpat)
		memmove(str + lensub, str + lenpat, lenstr + 1 - lenpat);

	memcpy(str, sub, lensub);
	return str + lensub;
}
