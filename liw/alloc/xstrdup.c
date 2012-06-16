/*
 * xstrdup.c -- error checking string duplication
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-alloc:$Id: xstrdup.c,v 1.1.1.1 1993/11/20 17:01:28 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "publib/alloc.h"
#include "publib/errormsg.h"

char *xstrdup(const char *s) {
	char *p;

	assert(s != NULL);
	p = strdup(s);
	if (p == NULL)
		__publib_error("strdup failed");
	return p;
}
