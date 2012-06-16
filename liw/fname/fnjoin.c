/*
 * fnjoin.c -- join two filenames together
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-fname:$Id: fnjoin.c,v 1.2 1994/06/20 20:49:23 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "publib/fname.h"

void fnjoin(char *res, const char *f1, const char *f2) {
	const char *p;

	assert(res != NULL);
	assert(f1 != NULL);
	assert(*f1 != '\0');
	assert(f2 != NULL);
	assert(*f2 != '\0');

	p = strchr(f1, '\0');
	if ((p > f1 && p[-1] == '/') || *f2 == '/')
		p = "";
	else
		p = "/";
	sprintf(res, "%s%s%s", f1, p, f2);
}
