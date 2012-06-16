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


static void delete(long from, long n, long newfrom, long newn) {
	printf("deleting %ld-%ld (-> mark1 should be: %ld-%ld)\n",
		from, from+n, newfrom, newfrom + newn);
	setup();
	sbuf_remark(mark2, from, n);
	sbuf_strchange(mark2, "", 0);
	dump();
}

static void insert(long at, const char *what, long newfrom, long newn) {
	printf("insert `%s' at %ld (-> mark1 should be: %ld-%ld)\n",
		what, at, newfrom, newfrom + newn);
	setup();
	sbuf_remark(mark2, at, 0);
	sbuf_strchange(mark2, what, strlen(what));
	dump();
}

static void search(long pos, const char *str, unsigned long opts) {
	int ret;
	setup();
	sbuf_remark(mark1, pos, sbuf_length(buf) - pos);
	ret = sbuf_search(mark2, mark1, str, strlen(str), opts);
	printf("search '%s' at %ld: ret=%d range=(%ld,%ld)\n", str, pos,
		ret, sbuf_mark_begin(mark2), sbuf_mark_length(mark2));
}

static void find_pair(long pos) {
	setup();
	printf("find_pair at %ld: ret=%ld\n", pos, sbuf_find_pair(buf, pos));
}

int main(void) {
	int i;
	printf("test-sbuf starting...\n");

	__set_liberror(__complain_on_error | __exit_on_error);

	setup();
	printf("character 4 (macro): %d\n", sbuf_charat(buf, 4));
	printf("character 4 (funcn): %d\n", (sbuf_charat)(buf, 4));
	
	delete(0, 2, 1, 3);	/* delete "ab" */
	delete(0, 3, 0, 3);	/* delete "abc" */
	delete(0, 4, 0, 2);	/* delete "abcd" */
	delete(3, 2, 3, 1);	/* delete "de" */
	delete(3, 3, 3, 0);	/* delete "def" */
	delete(3, 4, 3, 0);	/* delete "defg" */
	delete(5, 2, 3, 2);	/* delete "fg" */
	delete(6, 3, 3, 3);	/* delete "ghi" */
	delete(7, 2, 3, 3);	/* delete "hi" */

	for (i = 0; i < 3; ++i) insert(i, ".", 4, 3);
	for (i = 3; i < 6; ++i) insert(i, ".", 3, 4);
	for (i = 6; i < 10; ++i)insert(i, ".", 3, 3);

	search(0, "abc", 0);
	search(0, "def", 0);
	search(1, "...", 0);

	search(0, "abc", SBUF_BACKWARD);
	search(0, "def", SBUF_BACKWARD);
	search(1, "...", SBUF_BACKWARD);

	search(0, "ABC", SBUF_IGNORE_CASE);
	search(0, "DEF", SBUF_IGNORE_CASE);
	search(1, "...", SBUF_IGNORE_CASE);

	search(0, "ABC", SBUF_IGNORE_CASE | SBUF_BACKWARD);
	search(0, "DEF", SBUF_IGNORE_CASE | SBUF_BACKWARD);
	search(1, "...", SBUF_IGNORE_CASE | SBUF_BACKWARD);

	find_pair(0);
	find_pair(9);
	find_pair(11);
	find_pair(15);
	find_pair(17);
	
	{
	char foo[1000];
	int ret;
	setup();
	sbuf_remark(mark1, 0, 10);
	(void) sbuf_strchange(mark1, "abcdef\nqwerty\n", 14);
	                           /* 0123456 789abcd */
	sbuf_remark(mark1, 1, 11);
	sbuf_mark_set_columnar(mark1, 1);
	sbuf_strat(foo, mark1);
	printf("foo=<%s>\n", foo);
	ret = sbuf_strchange(mark1, "", 0);
	printf("ret == %d\n", ret);
	dump();
	}

	printf("undo:\n");	
	setup();
	dump();
	printf("ret=%d\n", sbuf_undo_atomic(buf));
	dump();

	sbuf_destroy(buf);

	printf("test-sbuf done\n");
	return 0;
}
