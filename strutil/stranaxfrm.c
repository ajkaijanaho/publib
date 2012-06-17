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
