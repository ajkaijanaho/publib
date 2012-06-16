/*
 * strend.c -- return pointer to the '\0' terminator of a string
 *
 * Part of publib.  See man page for more information.
 * "@(#)publib-strutil:$Id: strend.c,v 1.1 1994/06/20 20:30:11 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

char *strend(const char *s) {
	assert(s != NULL);
	return (char *)s + strlen(s);
}
