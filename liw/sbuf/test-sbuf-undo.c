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
