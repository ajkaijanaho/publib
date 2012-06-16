/*
 * xfclose.c -- error checking front-end for fclose
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-files:$Id: xfclose.c,v 1.2 1995/08/11 15:53:17 liw Exp $"
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "publib/errormsg.h"
#include "publib/files.h"

void xfclose(FILE *f) {
	if (ferror(f)) {
		__publib_error("An error during file I/O");
		exit(EXIT_FAILURE);
	}

	if (fclose(f) != 0) {
		__publib_error("An error while closeing a file");
		exit(EXIT_FAILURE);
	}
}
