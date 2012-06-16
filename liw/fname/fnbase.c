/*
 * fnbase.c -- find start of basename in filename
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-fname:$Id: fnbase.c,v 1.2 1994/06/20 20:47:47 liw Exp $"
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
