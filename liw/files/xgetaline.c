/*
 * xgetaline.c -- error checking front-end for getaline
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-files:$Id: xgetaline.c,v 1.2 1994/07/12 20:59:55 liw Exp $"
 */

#include <stdio.h>
#include <stdlib.h>

#include "publib/errormsg.h"
#include "publib/files.h"

char *xgetaline(FILE *fp) {
	char *p;

	p = getaline(fp);
	if (p == NULL && ferror(fp)) {
		__publib_error("getaline failed");
		exit(EXIT_FAILURE);
	}
	return p;
}
