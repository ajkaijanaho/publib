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
