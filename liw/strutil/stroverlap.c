/*
 * stroverlap.c -- check whether two strings overlap
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: stroverlap.c,v 1.1 1994/06/20 20:30:28 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

int stroverlap(const char *s, const char *t) {
	assert(s != NULL);
	assert(t != NULL);
	return memoverlap(s, strlen(s)+1, t, strlen(t)+1);
}
