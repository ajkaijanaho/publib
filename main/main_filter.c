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
 * main_filter.c -- process files for a typical UNIX filter.
 *
 * Lars Wirzenius.
 * Part of Publib.  See publib(3).
 * "@(#)publib-main:$Id: main_filter.c,v 1.3 2003/11/15 18:24:10 liw Exp $"
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
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
