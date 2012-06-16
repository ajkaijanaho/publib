/*
 * fnbase.c -- find start of basename in filename
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-fname:$Id: fnbase.c,v 1.1.1.1 1993/11/20 17:02:15 liw Exp $"
 */

#include <assert.h>
#include <string.h>

#include "publib/fname.h"

char *fnbase(const char *fname) {
	char *base;

	assert(fname != NULL);
	base = strrchr(fname, '/');
	if (base == NULL)
		return (char *) fname;
	return base+1;
}
