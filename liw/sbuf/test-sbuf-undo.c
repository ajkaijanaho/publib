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
 * test-sbuf-undo.c -- test undo parts of the sbuf routines
 */

#include <publib.h>

static Sbuf *buf;
static Sbufmark *mark1, *mark2;

static void setup(void) {
	if (buf != NULL) {
		sbuf_unmark(mark1);
		sbuf_unmark(mark2);
		sbuf_destroy(buf);
	}

	buf = sbuf_create();
	mark1 = sbuf_mark(buf, 0, 0);
	mark2 = sbuf_mark(buf, 0, 0);
}

static void dump(void) {
	char tmp[1024];

	printf("buf = `%s'\n", sbuf_lock(buf));
	sbuf_unlock(buf);

	sbuf_strat(tmp, mark1);
	printf("mark1 = %ld-%ld, len=%ld, text=`%s'\n",
		sbuf_mark_begin(mark1),
		sbuf_mark_end(mark1),
		sbuf_mark_length(mark1),
		tmp);

	sbuf_strat(tmp, mark2);
	printf("mark2 = %ld-%ld, len=%ld, text=`%s'\n",
		sbuf_mark_begin(mark2),
		sbuf_mark_end(mark2),
		sbuf_mark_length(mark2),
		tmp);

	printf("\n");
}


int main(void) {
	__set_liberror(__complain_on_error | __exit_on_error);

	setup();
	printf("initial:\n");
	dump();

	printf("change=%d\n", sbuf_strchange(mark1, "abc", 3));
	dump();

	printf("undo=%d\n", sbuf_undo_atomic(buf));
	dump();

	sbuf_destroy(buf);

	printf("test-sbuf done\n");
	return 0;
}
