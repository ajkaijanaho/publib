/*
 * fndelsuf.c -- delete last suffix from basename of filename
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-fname:$Id: fndelsuf.c,v 1.1.1.1 1993/11/20 17:02:15 liw Exp $"
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
