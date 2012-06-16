/*
 * File:	queue.c
 * Purpose:	Implement the queue abstract data type.
 * Author:	Lars Wirzenius
 * Version:	"@(#)publib:$Id: queue.c,v 1.1.1.1 1996/01/18 15:44:03 liw Exp $"
 */


#include <assert.h>
#include <stdlib.h>
#include "publib/queue.h"
#include "publib/alloc.h"
#include "publib/errormsg.h"



/*
 * Function:	queue_create
 * Purpose:	Create a new queue.
 * Arguments:	None.
 * Return:	Pointer to queue handle.
 */
Queue *queue_create(void) {
	Queue *q;

	q = malloc(sizeof(*q));
	if (q == NULL) {
		__publib_error("queue_create: out of memory");
		return NULL;
	}

	q->queue_front = NULL;
	q->queue_behind = NULL;

	return q;
}



/*
 * Function:	queue_destroy
 * Purpose:	Destroy a queue and its contents.
 * Arguments:	q	pointer to queue
 * Return:	Nothing.
 */
void queue_destroy(Queue *q) {
	assert(q != NULL);
	while (q->queue_front != NULL)
		(void) queue_remove(q);
	free(q);
}



/*
 * Function:	queue_front
 * Purpose:	Return foremost datum in queue.
 * Arguments:	q	pointer to queue
 * Return:	Pointer to data.
 */
void *queue_front(Queue *q) {
	assert(q != NULL);
	assert(q->queue_front != NULL);
	return q->queue_front->queue_data;
}



/*
 * Function:	queue_remove
 * Purpose:	Remove and return foremost datum in queue.
 * Arguments:	q	pointer to queue
 * Return:	Pointer to data.
 */
void *queue_remove(Queue *q) {
	struct queue_node *p;
	void *data;

	assert(q != NULL);
	assert(q->queue_front != NULL);

	p = q->queue_front;
	data = p->queue_data;

	q->queue_front = p->queue_previous;
	if (q->queue_front == NULL)
		q->queue_behind = NULL;

	free(p);

	return data;
}



/*
 * Function:	queue_is_empty
 * Purpose:	Is the queue empty?
 * Arguments:	q	pointer to queue
 * Return:	0 for not empty, 1 for empty.
 */
int queue_is_empty(Queue *q) {
	assert(q != NULL);
	return q->queue_front == NULL;
}



/*
 * Function:	queue_add
 * Purpose:	Add a new datum at end of queue.
 * Arguments:	q	pointer to queue
 *		data	pointer to data
 *		size	size of data
 * Description:	queue_add adds a new datum at the behind end of a queue.
 *		If `size' is 0, it only adds the pointer value `data'.
 *		Otherwise it makes a copy of the datum (first `size'
 *		bytes beginning at `data') and adds that into the queue.
 * 		In the latter case, when the datum is removed from the queue,
 *		it is the responsibility of the user to free the memory.
 * Return:	-1 for failure (memory allocation error), 0 for OK.
 */
int queue_add(Queue *q, const void *data, size_t size) {
	struct queue_node *p;

	p = malloc(sizeof(*p));
	if (p == NULL) {
		__publib_error("queue_add: out of memory");
		return -1;
	}

	if (size > 0) {
		data = memdup(data, size);
		if (data == NULL) {
			__publib_error("queue_add: out of memory");
			free(p);
			return -1;
		}
	}

	p->queue_previous = NULL;
	p->queue_data = (void *) data;

	if (q->queue_behind == NULL) {
		q->queue_behind = p;
		q->queue_front = p;
	} else {
		q->queue_behind->queue_previous = p;
		q->queue_behind = p;
	}

	return 0;
}
