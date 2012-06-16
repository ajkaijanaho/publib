/*
 * fndir.c -- copy all but basename of filename
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-fname:$Id: fndir.c,v 1.2 1994/06/20 20:49:21 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/fname.h"

void fndir(char *dir, const char *fname) {
	char *base;

	assert(dir != NULL);
	assert(fname != NULL);
	base = fnbase(fname);
	if (base == fname)
		strcpy(dir, ".");
	else {
		memcpy(dir, fname, (size_t)(base-fname));
		dir[base-fname] = '\0';
	}
}
