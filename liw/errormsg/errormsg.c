/*
 * errormsg.c -- print error message and optionally die
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-errormsg:$Id: errormsg.c,v 1.1.1.1 1995/08/06 21:57:17 liw Exp $"
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
	progname = (argv0 != NULL) ? argv0 : def;
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
