/*
 * strgsub.c -- substitute all occurences of pattern with another string
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strgsub.c,v 1.1.1.1 1994/02/03 17:25:30 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

int strgsub(char *str, const char *pat, const char *sub, size_t max) {
	size_t lenpat, lensub;
	const char *p;
	int n;

	assert(str != NULL);
	assert(pat != NULL);
	assert(*pat != '\0');
	assert(sub != NULL);
	assert(max >= strlen(str)+1);

	/*
	 * Check that the all substitutions will fit.
	 */
	lenpat = strlen(pat);
	lensub = strlen(sub);
	if (lenpat < lensub) {
		for (n = 0, p = str; (p = strstr(p, pat)) != NULL; p += lenpat)
			++n;
		if (strlen(str)+1 + n*(lensub-lenpat) > max)
			return -1;
	}

	/*
	 * Substitute.
	 */
	for (n = 0; (str = strsub(str, pat, sub)) != NULL; ++n)
		continue;
	return n;
}
