/*
 * File:	queue.h
 * Purpose:	Declarations for Publib's queue abstract data type.
 * Description:	A queue is a list-like data structure where all additions
 *		are done at one end (the behind of the queue) all deletions at
 *		the other end (the front of the queue).  It is very similar
 *		to a queue in a large food store, except that no-one tries
 *		to get in the queue anywhere except at the behind.
 * Note:	This Publib module reserves all names beginning with
 *		``queue'' or ``Queue''.
 * Author:	Lars Wirzenius
 * Version:	"@(#)publib:$Id: queue.h,v 1.1.1.1 1996/01/18 15:44:03 liw Exp $"
 */


#ifndef __publib_queue_h_included
#define __publib_queue_h_included

#include <stddef.h>
typedef struct queue Queue;

Queue *queue_create(void);
void queue_destroy(Queue *);
void *queue_front(Queue *);
void *queue_remove(Queue *);
int queue_is_empty(Queue *);
int queue_add(Queue *, const void *, size_t);

#ifdef __publib__
	struct queue_node {
		void *queue_data;
		struct queue_node *queue_previous;
	};
	
	struct queue {
		struct queue_node *queue_front, *queue_behind;
	};
#endif

#endif
