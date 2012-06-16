/*
 * strtabify.c -- convert runs of spaces and tabs to tabs (+ necessary spaces)
 *
 * Part of publib.  See man page for more information.
 * "@(#)publib-strutil:$Id: strtabify.c,v 1.1 1994/06/20 20:30:40 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "publib/strutil.h"


#define nexttab(col)	((col) + tabsize - (col)%tabsize)


char *strtabify(char *str, size_t tabsize) {
	size_t scol, tcol;
	char *s, *t;

	tcol = scol = 0;
	t = s = str;
	do {
		switch (*s) {
		case ' ':
			++scol;
			break;
		case '\t':
			scol = nexttab(scol);
			break;
		default:
			while (nexttab(tcol) <= scol) {
				*t++ = '\t';
				tcol = nexttab(tcol);
			}
			while (tcol < scol) {
				*t++ = ' ';
				++tcol;
			}
			*t++ = *s;
			++scol;
			tcol = scol;
			break;
		}
	} while (*s++ != '\0');

	return str;
}
