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
