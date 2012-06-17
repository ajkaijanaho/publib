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
