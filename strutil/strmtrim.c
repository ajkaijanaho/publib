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
