/*
 * strnlen.c -- give length of string, which needn't be zero terminated
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strnlen.c,v 1.1 1994/12/27 13:34:26 wirzeniu Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

size_t strnlen(const char *s, size_t n) {
	const char *p;

	p = memchr(s, '\0', n);
	if (p == NULL)
		return n;
	return p-s;
}
