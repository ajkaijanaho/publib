/*
 * strsplit.c -- split a string into "words"
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strsplit.c,v 1.3 1994/02/19 20:58:38 liw Exp $"
 */

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "publib/strutil.h"

static char *find_nonsep(const char *s, const char *sep);
static char *find_sep(const char *s, const char *sep);

int strsplit(char *s, char **words, int maxw, const char *sep) {
	char *start, *end;
	int count;

	assert(s != NULL);
	assert(words != NULL || maxw == 0);
	assert(sep != NULL);
	assert(!memoverlap(s, strlen(s)+1, sep, strlen(sep)+1));

	count = 0;
	end = s;
	while ((start = find_nonsep(end, sep)) != NULL) {
		end = find_sep(start, sep);
		if (count == maxw-1 && find_nonsep(end, sep) != NULL) {
			words[count] = start;
			
		}
		if (count < maxw) {
			*end++ = '\0';
			words[count] = start;
		}
		++count;
	}

	return count;
}


/* Find first character that is not a separator, starting with the character
   at s.  Return NULL if not found.  */
static char *find_nonsep(const char *s, const char *sep) {
	if (sep != NULL)
		s += strspn(s, sep);
	else
		while (isspace(*s))
			++s;
	return *s == '\0' ? NULL : (char *) s;
}


/* Find first character that is a separator, starting with the character
   at s.  Treat '\0' as a separator, so that the call always succeeds.  */
static char *find_sep(const char *s, const char *sep) {
	if (sep != NULL)
		s += strcspn(s, sep);
	else
		while (*s != '\0' && !isspace(*s))
			++s;
	return (char *) s;
}
