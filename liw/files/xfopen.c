/*
 * xfopen.c -- error checking front-end for fopen
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-files:$Id: xfopen.c,v 1.1.1.1 1993/11/20 17:02:01 liw Exp $"
 */

#include <stdio.h>
#include <errno.h>

#include "publib/errormsg.h"
#include "publib/files.h"

FILE *xfopen(const char *filename, const char *mode) {
	FILE *f;

	f = fopen(filename, mode);
	if (f == NULL) {
		/* can't use __publib_error here, because the message is
		   useless without the extra information */
		errormsg(__liberror & (__abort_on_error | __exit_on_error), 
			 errno, "xfopen: %s: can't open file in mode `%s'",
			 filename, mode);
	}
	return f;
}
