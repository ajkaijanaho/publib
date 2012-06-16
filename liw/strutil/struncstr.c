/*
 * struncstr.c -- convert C string literal notation into binary data
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: struncstr.c,v 1.3 1995/08/11 15:52:07 liw Exp $"
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
			while (++i < 3 && isdigit(*str))
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
