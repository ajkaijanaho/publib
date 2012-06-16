/*
 * File:	priq.c
 * Purpose:	Implementation of priority queues.
 * Author:	Lars Wirzenius
 * Version:	"@(#)publib:$Id: priq.c,v 1.1.1.1 1996/01/18 17:55:38 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "publib/priq.h"
#include "publib/errormsg.h"



/*
 * Prototypes for local functions.
 */

static int always_same(const void *, const void *);
static void percolate_down(Priq *, size_t);



/*
 * Function:	priq_create
 * Purpose:	Create a new priority queue.
 * Arguments:	compare	comparison function for data elements
 *			(NULL means all elements are equal)
 * Return:	Pointer to new queue, NULL on error (memory allocation).
 */
Priq *priq_create(int (*compare)(const void *, const void *)) {
	Priq *pq;

	pq = malloc(sizeof(*pq));
	if (pq == NULL) {
		__publib_error("priq_create: out of memory");
		return NULL;
	}

	dynarr_init(&pq->da, sizeof(void *));
	pq->compare = compare ? compare : always_same;

	return pq;
}



/*
 * Function:	priq_destroy
 * Purpose:	Destroy a priority queue.
 * Arguments:	pq	pointer to priority queue
 * Return:	None.
 */
void priq_destroy(Priq *pq) {
	assert(pq != NULL);
	dynarr_free(&pq->da);
	free(pq);
}



/*
 * Function:	priq_is_empty
 * Purpose:	Is a priority queue empty?
 * Arguments:	pq	pointer to priority queue
 * Return:	0 for not empty, 1 for empty.
 */
int priq_is_empty(Priq *pq) {
	assert(pq != NULL);
	return pq->da.used == 0;
}



/*
 * Function:	priq_insert
 * Purpose:	Insert new element into priority queue.
 * Arguments:	pq	pointer to priority queue
 *		data	pointer to data
 *		size	size of data
 * Return:	-1 for failure, 0 for success.
 */
int priq_insert(Priq *pq, const void *data, size_t size) {
	void **arr;
	size_t i;

	assert(pq != NULL);

	if (dynarr_resize(&pq->da, pq->da.used + 1) == -1)
		return -1;

	if (size > 0) {
		data = memdup(data, size);
		if (data == NULL) {
			__publib_error("priq_insert: out of memory");
			return -1;
		}
	}

	arr = pq->da.data;
	++pq->da.used;

	if (pq->da.used == 1)
		i = 0;
	else {
		i = pq->da.used - 1;
		while (i > 0 && pq->compare(arr[i/2], data) > 0) {
			arr[i] = arr[i/2];
			i /= 2;
		}
	}

	arr[i] = (void *) data;

	return 0;
}



/*
 * Function:	priq_remove
 * Purpose:	Remove and return smallest element from priority queue.
 * Arguments:	pq	pointer to priority queue
 * Return:	Pointer to data.
 */
void *priq_remove(Priq *pq) {
	void *smallest, **arr;

	assert(pq != NULL);
	assert(!priq_is_empty(pq));

	arr = pq->da.data;
	smallest = arr[0];
	--pq->da.used;
	if (pq->da.used > 0) {
		arr[0] = arr[pq->da.used];
		percolate_down(pq, 0);
	}

	return smallest;
}



/*
 * Function:	priq_set_compare
 * Purpose:	Change the comparison function and reorder the priority queue.
 * Arguments:	pq	pointer to priority queue
 *		compare	new comparison function
 * Return:	Nothing.
 */
void priq_set_compare(Priq *pq, int (*compare)(const void *, const void *)) {
	size_t i;

	assert(pq != NULL);

	pq->compare = compare ? compare : always_same;

	for (i = pq->da.used / 2; i-- > 0; )
		percolate_down(pq, i);
}



/*
 * Function:	priq_dump
 * Purpose:	Dump contents of priority queue to stdout.
 * Arguments:	pq	pointer to priority queue
 * Return:	Nothing.
 * Note:	Assumes elements are pointers to integers.
 */
void priq_dump(Priq *pq) {
	size_t i;
	int **arr;

	printf("priq:");
	arr = pq->da.data;
	for (i = 0; i < pq->da.used; ++i)
		printf(" %d", *arr[i]);
	printf("\n");
}



/**********************************************************************
 * Local functions follow
 */



/*
 * Function:	always_same
 * Purpose:	Comparison function that always returns "same".
 * Note:	See qsort(3) for a description of comparison functions.
 */
int always_same(const void *a, const void *b) {
	return 0;
}



/*
 * Function:	percolate_down
 * Purpose:	Make sure a subtree in a priority queue is ordered correctly.
 * Arguments:	pq	pointer to priority queue
 *		i	root element of subtree
 * Note:	See Weiss, ``Data Structures and Analysis'', page 181, for
 *		a description.
 * Note:	Only the subtree root may be in wrong place.
 */
static void percolate_down(Priq *pq, size_t i) {
	size_t n, child;
	void *x, **arr;

	arr = pq->da.data;
	n = pq->da.used-1;
	assert(i <= n);
	x = arr[i];
	while ((child = i*2+1) <= n) {
		if (child != n && pq->compare(arr[child+1], arr[child]) < 0)
			++child;
		if (pq->compare(x, arr[child]) <= 0)
			break;
		arr[i] = arr[child];
		i = child;
	}
	arr[i] = x;
}
