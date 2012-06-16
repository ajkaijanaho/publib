/*
 * main_filter.c -- process files for a typical UNIX filter.
 *
 * Lars Wirzenius.
 * Part of Publib.  See publib(3).
 * "@(#)publib-main:$Id: main_filter.c,v 1.2 1996/01/07 22:02:58 liw Exp $"
 */

#include <assert.h>
#include <stdio.h>
#include "publib/errormsg.h"
#include "publib/main.h"

int main_filter(int n, char **v, int (*fun)(FILE *, char *, void *), void *x) {
	FILE *f;
	int i, ret;
	
	assert(n >= 0);
	assert(v != NULL);
	assert(fun != NULL);
#ifndef NDEBUG
	for (i = 0; i < n; ++i) assert(v[i] != NULL);
#endif
	
	ret = 0;
	
	if (n == 0)
		ret = fun(stdin, "stdin", x);
	else {
		for (i = 0; i < n; ++i) {
			if (strcmp(v[i], "-") == 0)
				f = stdin;
			else {
				f = fopen(v[i], "r");
				if (f == NULL) {
					errormsg(0, -1, "couldn't open "
						"file `%s' for reading", 
						v[i]);
					continue;
				}
			}
			if (fun(f, v[i], x) == -1)
				ret = -1;
			if (ferror(f)) {
				errormsg(0, -1, "error with file `%s'", v[i]);
				ret = -1;
			}
			if (f != stdin && fclose(f) == EOF) {
				errormsg(0, -1, "error closing file `%s'",
					v[i]);
			}
		}
	}
	return ret;
}
