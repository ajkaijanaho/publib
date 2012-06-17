/* Part of publib.

   Copyright (c) 2012 Antti-Juhani Kaijanaho.
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
 * test-sbuf.c -- test various parts of the sbuf routines
 *
 * At the moment, only one test is done: if we have the following text:
 *
 *	"abcdefghi"
 *
 * and two marks ("def" and the other variable), and change the text of
 * one mark, is the other updated correctly?
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

	sbuf_strchange(mark1, "abcd\377fghi(x[123]y)abcdefghi", 27);
	sbuf_remark(mark1, 3, 3);
}

static void dump(void) {
	sbuf_dump("buf", buf);
	sbuf_mark_dump("mark1", mark1);
	sbuf_mark_dump("mark2", mark2);
}


int main(void) {
	__set_liberror(__complain_on_error | __exit_on_error);

	{
	char foo[1000];
	int ret;

	setup();
	sbuf_remark(mark1, 0, sbuf_length(buf));
	(void) sbuf_strchange(mark1, "abcdef\nghijkl\nmnopqr\nstuvwx\n", 28);
	                           /* 0123456 789abcd */
	sbuf_remark(mark1, 1, 11);
	sbuf_mark_set_columnar(mark1, 1);
	(void)sbuf_mark_length(mark1);
	sbuf_strat(foo, mark1);
	sbuf_remark(mark2, 14, 0);
	sbuf_mark_set_columnar(mark2, 1);
	dump();

	ret = sbuf_change(mark2, mark1);
	printf("ret == %d\n", ret);

	dump();
	}

	sbuf_destroy(buf);

	return 0;
}
