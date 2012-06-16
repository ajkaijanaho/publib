/*
 * File:	test-queue.c
 * Purpose:	Test the queue data structure routines.
 * Author:	Lars Wirzenius
 * Version:	"@(#)publib:$Id: test-queue.c,v 1.1.1.1 1996/01/18 15:44:03 liw Exp $"
 */

#include <stdio.h>
#include <publib.h>

int main(void) {
	Queue *q;
	char *str[] = {
		"hello, world", "ciao, bambino", "hasta la vista, baby",
		"I'll be back", "you talking to me?",
	};
	const int n = sizeof(str) / sizeof(*str);
	int i;

	__set_liberror(__complain_on_error | __exit_on_error);

	q = queue_create();
	if (!queue_is_empty(q))
		errormsg(1, 0, "queue not empty after creation");

	for (i = 0; i < n; ++i) {
		(void) queue_add(q, str[i], 0);
		if (queue_is_empty(q))
			errormsg(1, 0, "queue is empty after add");
	}

	for (i = 0; i < n; ++i)
		if (queue_remove(q) != str[i])
			errormsg(1, 0, "queue not in order");
	if (!queue_is_empty(q))
		errormsg(1, 0, "queue is not empty after removals");

	queue_destroy(q);

	printf("queue seems to work\n");
	return 0;
}
