/*
 * strndup.c -- duplicate at most a given beginning of a string
 *
 * Part of publib.  See man page for more information.
 * "@(#)publib-strutil:$Id: strndup.c,v 1.1 1994/06/20 20:30:24 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "publib/strutil.h"
#include "publib/errormsg.h"

char *strndup(const char *str, size_t n) {
	char *dup;
	size_t len;

	len = strlen(str);
	if (n > len)
		n = len;

	dup = malloc(n+1);
	if (dup == NULL) {
		__publib_error("malloc failed");
		return NULL;
	}

	memcpy(dup, str, n);
	dup[n] = '\0';
	return dup;
}
