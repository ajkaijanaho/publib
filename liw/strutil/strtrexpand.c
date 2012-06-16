/*
 * strtrexpand.c -- expand tr-like notation in string
 *
 * Part of publib.  See man page for more information.
 * "@(#)publib-strutil:$Id: strtrexpand.c,v 1.1 1994/06/20 20:30:43 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "publib/strutil.h"


/* a-b: everything from charcode a to charcode b
   \a: the character a
   \o \oo \ooo : character with octal code o
*/
#define odigit(c)	(c >= '0' && c <= '7')
void strtrexpand(char *tgt, const char *src) {
	int c;
	while (src[0] != '\0') {
		if (src[0] == '\\' && odigit(src[1])) {
			++src;
			c = *src++ - '0';
			if (odigit(*src))
				c = c*8 + *src++ - '0';
			if (odigit(*src))
				c = c*8 + *src++ - '0';
			*tgt++ = c;
		} else if (src[0] == '\\') {
			*tgt++ = src[1];
			src += 2;
		} else if (src[1] == '-' && src[2] != '\0') {
			for (c = src[0]; c <= src[2]; ++c)
				*tgt++ = c;
			src += 3;
		} else {
			*tgt++ = *src++;
		}
	}
	*tgt = '\0';
}
