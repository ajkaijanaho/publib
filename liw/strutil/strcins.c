/*
 * strcins.c -- insert a char at the beginning of a string
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strcins.c,v 1.1 1994/07/22 21:01:27 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

void strcins(char *str, int c) {
	assert(str != NULL);

	memmove(str+1, str, strlen(str)+1);
	*str = c;
}
