/*
 * fnsetsuf.c -- replace (last) suffix of filename
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-fname:$Id: fnsetsuf.c,v 1.2 1994/06/20 20:49:27 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/fname.h"

size_t fnsetsuf(char *fname, const char *suffix, size_t max) {
	char *dot;
	size_t needed;

	assert(fname != NULL);
	assert(suffix != NULL);
	assert(max > 0);

	dot = fnlastsuf(fname);
	if (dot == NULL) {
		needed = strlen(fname) + strlen(suffix) + 1;
		if (needed > max)
			return needed;
		strcat(fname, suffix);
	} else {
		needed = (strlen(fname) - strlen(dot)) + strlen(suffix) + 1;
		if (needed > max)
			return needed;
		strcpy(dot, suffix);
	}

	return needed;
}
