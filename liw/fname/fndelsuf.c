/*
 * fndelsuf.c -- delete last suffix from basename of filename
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-fname:$Id: fndelsuf.c,v 1.2 1994/06/20 20:49:20 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/fname.h"

int fndelsuf(char *fname, const char *suffix) {
	char *dot;

	assert(fname != NULL);
	assert(suffix != NULL);
	assert(*suffix != '\0');
	dot = fnlastsuf(fname);
	if (dot == NULL)
		return -1;
	if (suffix == NULL || strcmp(dot, suffix) == 0) {
		*dot = '\0';
		return 0;
	}
	return -1;
}
