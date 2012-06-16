/*
 * struntabify.c -- convert tabs to spaces
 *
 * Part of publib.  See man page for more information.
 * "@(#)publib-strutil:$Id: struntabify.c,v 1.1 1994/06/20 20:30:45 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "publib/strutil.h"


char *struntabify(char *str, size_t tabsize) {
	size_t n;
	char *s;

	assert(str != NULL);
	assert(tabsize > 0);

	s = str;
	while ((s = strchr(s, '\t')) != NULL) {
		n = tabsize - (s-str)%tabsize;
		memmove(s+1 + n-1, s+1, strlen(s+1)+1);
		memset(s, ' ', n);
		s += n;
	}
	return str;
}
