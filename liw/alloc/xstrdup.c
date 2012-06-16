/*
 * xstrdup.c -- error checking string duplication
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-alloc:$Id$"
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
