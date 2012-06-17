/* Part of publib.

   Copyright (c) 1994-2006 Lars Wirzenius.  All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
	char *start, *end, *endstr;
	int count;

	assert(s != NULL);
	assert(words != NULL || maxw == 0);
	assert(sep != NULL);
	assert(!memoverlap(s, strlen(s)+1, sep, strlen(sep)+1));

	count = 0;
	end = s;
	endstr = strchr(s, '\0');
	
        while (end < endstr) {
                start = find_nonsep(end, sep);
                if (start == NULL)
                        break;
                end = find_sep(start, sep);
                *end++ = '\0';
                if (count < maxw)
                        words[count] = start;
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
