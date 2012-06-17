/* Part of publib.

   Copyright (c) 1994-2006 Lars Wirzenius.  All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*
 * errormsg.c -- print error message and optionally die
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-errormsg:$Id: errormsg.c,v 1.2 1999/03/24 22:29:13 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "publib/errormsg.h"

static const char *progname = NULL;

enum __liberror __liberror = __exit_on_error;

void __set_liberror(enum __liberror i) {
	__liberror = i;
}


/* 
 * Set program name; this is automatically included in the error messages.
 * Both arguments should point at strings that have static duration, i.e.
 * they exist until the program terminates (or at least until the last error
 * message has been printed); this is so that it is not necessary to create
 * a copy of the name.  (The arguments can also be NULL.)
 *
 * If the first argument is non-NULL, that is used, otherwise the seconds
 * argument is used.  If both are NULL, no program name is included in the
 * output.  The reason for having two arguments is so that the caller
 * doesn't have to do the test, and can just call 
 *	set_progname(argv[0], "default_name");
 * (it is valid for argv[0] to be NULL, under ISO C).
 */
void set_progname(const char *argv0, const char *def) {
	if (argv0 != NULL && *argv0 != '\0')
		progname = argv0;
	else if (def != NULL && *def != '\0')
		progname = def;
}


/*
 * Return program name set by set_progname, or an empty string if not set.
 */
const char *get_progname(void) {
	return progname != NULL ? progname : "";
}


/*
 * Print error message; if exitp is 1, exit, if 2, abort, after printing
 * the message.  If eno is positive, print error corresponding strerror
 * message; if eno is -1, use current value of errno.
 */
void errormsg(int do_after, int eno, const char *fmt, ...) {
	va_list args;
	size_t len;
	int print_newline, print_progname;

	if (eno == -1)		/* this must be done before anything else */
		eno = errno;

	assert(do_after == 0 || do_after == 1 || do_after == 2);
	assert(eno >= -1);	/* there is no known maximum value for errno */
	assert(fmt != NULL);
		
	fflush(NULL);		/* output all pending output */

	print_progname = (progname != NULL) && (*progname != '\0');

	if (print_progname)
		fprintf(stderr, "%s: ", progname);

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	len = strlen(fmt);
	print_newline = (len > 0) && (fmt[len-1] == '\n');

	if (eno != 0) {
		if (print_newline) {
			if (print_progname)
				fprintf(stderr, "%s: ", progname);
			fprintf(stderr, "Possible explanation: ");
		} else {
			fprintf(stderr, ": ");
		}
		fprintf(stderr, "%s\n", strerror(eno));
	} else {
		if (!print_newline)
			fprintf(stderr, "\n"); /* make sure newline ends msg */
	}

	fflush(stderr);

	switch (do_after) {
	case 1:
		exit(EXIT_FAILURE);
	case 2:
		abort();
	}
}
