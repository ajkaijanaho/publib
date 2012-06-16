/*
 * strrot13.c -- encrypt string with rot13
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strrot13.c,v 1.1.1.1 1994/02/03 17:25:31 liw Exp $"
 */

#include <assert.h>
#include "publib/strutil.h"

#define N	26


/* Warning: this code assumes ASCII */
char *strrot13(char *str) {
	char *s = str;

	while (*s != '\0') {
		if ((*s >= 'a' && *s <= 'm') || (*s >= 'A' && *s <= 'M'))
			*s += 13;
		else if ((*s >= 'n' && *s <= 'z') || (*s >= 'N' && *s <= 'Z'))
			*s -= 13;
		++s;
	}
	return str;
}
