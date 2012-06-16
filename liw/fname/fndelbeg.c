/*
 * fndelbeg.c -- delete first part of filename
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-fname:$Id: fndelbeg.c,v 1.2 1994/06/20 20:49:18 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/fname.h"

void fndelbeg(char *fname) {
	char *p;

	assert(fname != NULL);
	if (*fname == '/')
		memmove(fname, fname+1, strlen(fname)+1);
	else {
		p = strchr(fname, '/');
		if (p == NULL)
			*fname = '\0';
		else
			memmove(fname, p, strlen(p) + 1);
	}
}
