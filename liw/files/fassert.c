/*
 * fassert.c -- check for errors on stream
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-files:$Id: fassert.c,v 1.2 1995/08/11 15:53:16 liw Exp $"
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
