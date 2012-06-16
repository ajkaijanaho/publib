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
	long len;
	int ret;

	setup();
	sbuf_remark(mark1, 0, sbuf_length(buf));
	(void) sbuf_strchange(mark1, "abcdef\nghijkl\nmnopqr\nstuvwx\n", 28);
	                           /* 0123456 789abcd */
	sbuf_remark(mark1, 1, 11);
	sbuf_mark_set_columnar(mark1, 1);
	len = sbuf_mark_length(mark1);
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
