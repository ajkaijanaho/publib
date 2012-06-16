/*
 * xfseek.c -- error checking front-end for fseek
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-files:$Id: xfseek.c,v 1.1.1.1 1993/11/20 17:02:01 liw Exp $"
 */

#include <stdio.h>
#include <stdlib.h>

#include "publib/errormsg.h"
#include "publib/files.h"

void xfseek(FILE *f, long off, int origin) {
	if (fseek(f, off, origin) != 0) {
		__publib_error("fseek failed");
		exit(EXIT_FAILURE);
	}
}
