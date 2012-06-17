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
 * File:	test-priq.c
 * Purpose:	Test priority queue routines.
 * Author:	Lars Wirzenius
 * Version:	"@(#)publib:$Id: test-priq.c,v 1.1.1.1 1996/01/18 17:55:38 liw Exp $"
 */

#include <stdio.h>
#include <stdlib.h>
#include <publib.h>

#define N 10

static int ascending(const void *a, const void *b) {
	const int aa = *(const int *) a;
	const int bb = *(const int *) b;

	if (aa < bb)
		return -1;
	if (aa > bb)
		return 1;
	return 0;
}

static int descending(const void *a, const void *b) {
	return -ascending(a, b);
}

static void build(int *arr, int n) {
	while (n-- > 0)
		*arr++ = rand() % 10;
}

static void dump(int *arr, int n) {
#if 0
	printf("arr:");
	while (n-- > 0)
		printf(" %d", *arr++);
	printf("\n");
#endif
}

static void check_order(Priq *pq, int n, int (*o)(const void *,const void *)) {
	int *p, *prev;

	prev = priq_remove(pq);
	while (n-- > 1) {
		p = priq_remove(pq);
		if (o(p, prev) < 0)
			errormsg(1, 0, "priority queue not in order");
		prev = p;
	}
	if (!priq_is_empty(pq))
		errormsg(1, 0, "priority queue is not empty after removals");
}

static void check_insert(Priq *pq, int *arr, int n) {
	while (n-- > 0) {
		priq_insert(pq, arr, 0);
		if (priq_is_empty(pq))
			errormsg(1, 0, "priority queue empty after insertion");
		++arr;
	}
}

int main(void) {
	int arr[N];
	Priq *pq;

	__set_liberror(__complain_on_error | __exit_on_error);

	pq = priq_create(ascending);
	if (!priq_is_empty(pq))
		errormsg(1, 0, "priority queue not empty after creation");

	build(arr, N);
	dump(arr, N);
	check_insert(pq, arr, N);
	check_order(pq, N, ascending);

	build(arr, N);
	dump(arr, N);
	check_insert(pq, arr, N);
	priq_set_compare(pq, descending);
	check_order(pq, N, descending);

	printf("priority queues seem to work\n");
	return 0;
}
