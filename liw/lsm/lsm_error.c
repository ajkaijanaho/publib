/*
 * lsm_error.c -- error messages for lsm
 *
 * Lars Wirzenius
 * Part of Publib, see publib(3) for more information.
 * "@(#)publib-lsm:$Id: lsm_error.c,v 1.2 1995/11/26 10:13:12 liw Exp $"
 */
 
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "publib/strutil.h"
#include "publib/lsm.h"

#define MAX_ERRORS	128
#define MAX_ERROR_LEN	1024
static char errors[MAX_ERRORS][10240];
static int first = 0;
static int next = 0;

void lsm_set_error(long lineno, int errno, const char *fmt, ...) {
	va_list args;
	
	va_start(args, fmt);
	if (lineno > 0)
		sprintf(errors[next], "%ld: ", lineno);
	vsprintf(strend(errors[next]), fmt, args);
	if (errno != 0)
		sprintf(strend(errors[next]), ": %s", strerror(errno));
	next = (next + 1) % MAX_ERRORS;
	if (next == first)
		first = (first + 1) % MAX_ERRORS;
	va_end(args);
}


char *lsm_error(void) {
	char *p;
	
	if (first == next)
		p = NULL;
	else {
		p = errors[first];
		first = (first + 1) % MAX_ERRORS;
	}
	return p;
}
