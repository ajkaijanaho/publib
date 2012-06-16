/*
 * xgetaline.c -- error checking front-end for getaline
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-files:$Id: xgetaline.c,v 1.3 1995/08/11 15:53:18 liw Exp $"
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
