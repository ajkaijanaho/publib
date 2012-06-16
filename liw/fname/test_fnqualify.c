/*
 * test_fnqualify.c -- test fnqualify
 *
 * Part of publib.
 * "@(#)publib-fname:$Id: test_fnqualify.c,v 1.1.1.1 1993/11/20 17:02:15 liw Exp $"
 */

#include <stdio.h>

#include "publib/fname.h"

int main(int argc, char **argv) {
	int i;
	char buf[FILENAME_MAX];
	size_t n;

	for (i = 1; i < argc; ++i) {
		n = fnqualify(buf, argv[i], sizeof(buf));
		printf("input:  `%s'\n", argv[i]);
		printf("output: `%s'\n", buf);
		printf("n: %lu\n", (unsigned long) n);
		printf("\n");
	}

	return 0;
}
