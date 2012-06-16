/*
 * xfseek.c -- error checking front-end for fseek
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-files:$Id: xfseek.c,v 1.2 1995/08/11 15:53:17 liw Exp $"
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
