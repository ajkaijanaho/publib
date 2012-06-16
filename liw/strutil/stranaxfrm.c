/*
 * stranaxfrm.c -- make a string into canonical order for anagram comparison
 *
 * Part of publib.  See man page for more information.
 * "@(#)publib-strutil:$Id: stranaxfrm.c,v 1.1 1994/06/20 20:30:06 liw Exp $"
 */

#include <assert.h>
#include "publib/strutil.h"

char *stranaxfrm(char *str) {
	char c, *s, *t, *min;

	assert(str != NULL);

	/*
	 * Sort the letters in the string, using selection sort.  While 
	 * slowish for long strings, it should be quite fast enough for
	 * the usual short ones.  (Beep! Untested efficiency claim!)
	 */
	for (s = str; *s != '\0'; ++s) {
		for (min = t = s; *t != '\0'; ++t)
			if (*t < *min)
				min = t;
		c = *s;
		*s = *min;
		*min = c;
	}
	return str;
}
