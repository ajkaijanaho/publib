/*
 * strshuffle.c -- make the characters in a string be in random order
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: strshuffle.c,v 1.1 1994/06/20 20:30:38 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "publib/strutil.h"


char *strshuffle(char *str) {
	assert(str != NULL);
	return memshuffle(str, 1, strlen(str));
}
