/*
 * add.c -- add numbers to an iset, then print out minimum ranges
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id: test_add.c,v 1.1.1.1 1993/11/20 17:02:31 liw Exp $"
 */

#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "publib/iset.h"

void dump(Iset *is) {
	struct __iset_range *r;

	printf("dump:");
	for (r = is->lowest; r != NULL; r = r->next)
		printf(" %ld-%ld", r->start, r->end);
	printf("\n");
}

int main(void) {
	int i;
	Iset *is;

	assert((is = iset_create()) != NULL);
	dump(is);
	while (scanf("%d", &i) == 1) {
		assert(iset_add(is, i) != -1);
		dump(is);
	}
	iset_destroy(is);

	exit(0);
}
