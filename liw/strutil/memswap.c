/*
 * memswap.c -- swap contents of two memory blocks
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: memswap.c,v 1.2 1994/02/05 17:08:43 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"

/* The size of the auxiliary array that is used while swapping */
#define N	1024

void memswap(void *block1, void *block2, size_t n) {
	char *s, *t, buf[N];

	assert(block1 != NULL);
	assert(block2 != NULL);
	assert(!memoverlap(block1, n, block2, n));

	s = block1;
	t = block2;
	while (n > N) {
		memcpy(buf, s, N);
		memcpy(s, t, N);
		memcpy(t, buf, N);
		s += N;
		t += N;
		n -= N;
	}
	memcpy(buf, s, n);
	memcpy(s, t, n);
	memcpy(t, buf, n);
}
