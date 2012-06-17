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
 * strcstr.c -- convert binary data into C string literal notation
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strcstr.c,v 1.2 1994/02/05 17:08:43 liw Exp $"
 */

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include "publib/strutil.h"


/* Append a character to the end of the result string if it fits.  */
#define append(c)	(void)((m < max-1) && (str[m++] = (c)))


void strcstr(char *str, size_t max, const void *block, size_t n) {
	static const char print[] = "'\"?\\abfnrtv";
	static const char unprint[] = "'\"?\\\a\b\f\n\r\t\v";
	int i, thishex, prevhex;
	const char *p, *data;
	size_t m;

	assert(str != NULL);
	assert(max > 0);
	assert(block != NULL);
	assert(!memoverlap(str, max, block, n));

	data = block;
	m = 0;
	prevhex = 0;
	while (m < max-1 && n-- > 0) {
		thishex = 0;
		if (*data == '\0') {
			append('\\');
			append('0');
			if (isdigit(data[1])) {
				append('0');
				append('0');
			}
			++data;
		} else if ((p = strchr(unprint, *data)) != NULL) {
			append('\\');
			append(print[p-unprint]);
			++data;
		} else if (isprint(*data) && !(prevhex && isxdigit(*data)))
			append(*data++);
		else {
			append('\\');
			append('x');
			i = (CHAR_BIT/4+1)*4-4;
			while (m < max-1 && i >= 0) {
				append("0123456789abcdef"[(*data & (0xf << i)) >> i]);
				i -= 4;
			}
			thishex = 1;
			++data;
		}
		prevhex = thishex;
	}
	append('\0');
}
