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
 * test-main-filter.c -- test the main_filter function
 *
 * Lars Wirzenius.
 * Part of Publib.  See publib(3).
 * "@(#)publib-main:$Id: test-main-filter.c,v 1.1 1995/12/05 21:33:55 liw Exp $"
 */
 
#include <stdio.h>
#include <publib.h>

static int count_lines(FILE *f, char *name, void *totalptr) {
	long lines;
	int c;
	
	lines = 0;
	while ((c = getc(f)) != EOF)
		if (c == '\n')
			++lines;
	*((long *) totalptr) += lines;
	if (ferror(f))
		return -1;
		
	printf("%6ld %s\n", lines, name);
	return 0;
}

int main(int argc, char **argv) {
	long total_lines = 0;
	int ret;
	set_progname(argv[0], "test-main-filter");
	ret = main_filter(argc-1, argv+1, count_lines, &total_lines);
	printf("main_filter returned %d, total_lines=%ld\n", ret, 
		total_lines);
	return 0;
}
