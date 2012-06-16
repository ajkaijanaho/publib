/*
 * fassert.c -- check for errors on stream
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-files:$Id: fassert.c,v 1.1.1.1 1993/11/20 17:02:01 liw Exp $"
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "publib/errormsg.h"
#include "publib/files.h"

void fassert(FILE *fp) {
	if (fp == NULL) {
		__publib_error("argument is NULL");
		exit(EXIT_FAILURE);
	}

	if (ferror(fp)) {
		__publib_error("I/O error on file");
		exit(EXIT_FAILURE);
	}
}
