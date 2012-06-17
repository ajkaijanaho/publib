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
 * struncstr.c -- convert C string literal notation into binary data
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: struncstr.c,v 1.4 2004/07/11 10:19:19 liw Exp $"
 */

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "publib/strutil.h"


/* Append a character to the end of the data block, if it fits. */
#define append(c)	(void)(m < max && (data[m++] = (c)))

/* Convert hexadecimal character to its corresponding integer value.
   Assume that the character is valid.  */
#define hextoint(c)	hexvalues[strchr(hexdigits, (c))-hexdigits];


void struncstr(void *block, const char *str, size_t max) {
	static const char print[] = "'\"?\\abfnrtv";
	static const char unprint[] = "'\"?\\\a\b\f\n\r\t\v";
	static const char hexdigits[] = "0123456789abcdefABCDEF";
	static const char hexvalues[] = "\0\1\2\3\4\5\6\7\x8\x9"
					"\xa\xb\xc\xd\xe\xf"
					"\xA\xB\xC\xD\xE\xF";
	char *p, *data;
	unsigned c;
	size_t m;
	int i;

	assert(str != NULL);
	assert(block != NULL);
	assert(!memoverlap(block, max, str, strlen(str)+1));

	data = block;
	m = 0;
	while (m < max && *str != '\0') {
		if (*str != '\\')		/* printable character? */
			append(*str++);
		else if (str[1] == 'x') {	/* hex escape sequence? */
			str += 2;
			c = 0;
			while (isxdigit(*str)) {
				c = (c << 4) | hextoint(*str);
				++str;
			}
			append(c);
		} else if (isdigit(str[1])) {	/* octal escape sequence? */
			++str;
			c = i = 0;
			while (i++ < 3 && isdigit(*str))
				c = (c << 3) | hextoint(*str++);
			append(c);
		} else if ((p = strchr(print, str[1])) != NULL) {
			append(unprint[p-print]); /* simple esc sequence */
			str += 2;
		} else {			/* undefined sequence! */
			append('\\');
			append(str[1]);
			str += 2;
		}
	}
}
