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
