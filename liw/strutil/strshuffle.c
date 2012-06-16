/*
 * strshuffle.c -- make the characters in a string be in random order
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: memshuffle.c,v 1.2 1994/02/19 20:58:36 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "publib/strutil.h"


char *strshuffle(char *str) {
	assert(str != NULL);
	return memshuffle(str, 1, strlen(str));
}
