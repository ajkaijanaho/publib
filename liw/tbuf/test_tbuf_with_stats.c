/*
 * test_tbuf_with_stats.c -- do tbuf buffer ops given in input, give stats

for every input line:
	Read description of operation from stdin
	apply it
	print statistics of new tree
	delete old tree

operations:
	offset length newlength
		replace offset..(length-1) with random text of length newlength

statistics:
	length of buffer
	height of chunk tree
	nodes in tree
	number of chunks in tree
	total amount of memory used by tree

 *
 * Part of Publib, see man page for more information.
 * "@(#)publib-tbuf:$Id: test_tbuf_with_stats.c,v 1.3 1998/11/14 21:17:37 liw Exp $"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "publib/tbuf.h"

static void panic(const char *msg) {
	fprintf(stderr, "PANIC: %s\n", msg);
	exit(EXIT_FAILURE);
}

static Tbuf *x_tbuf_create(const char *str, size_t len) {
	Tbuf *tbuf;
	
	tbuf = tbuf_create(str, len);
	if (tbuf == NULL)
		panic("tbuf_create returned NULL");
	return tbuf;
}

static Tbuf *x_tbuf_copy(Tbuf *tbuf, size_t offset, size_t len) {
	Tbuf *tbuf2;
	
	tbuf2 = tbuf_copy(tbuf, offset, len);
	if (tbuf2 == NULL)
		panic("tbuf_copy returned NULL");
	return tbuf2;
}

static Tbuf *x_tbuf_cat(Tbuf *tbuf1, Tbuf *tbuf2) {
	Tbuf *tbuf;
	
	tbuf = tbuf_cat(tbuf1, tbuf2);
	if (tbuf == NULL)
		panic("tbuf_cat returned NULL");
	return tbuf;
}

static char *x_make_text(size_t wanted) {
	static char *buf = NULL;
	static size_t len = 0;
	
	if (wanted > len) {
		buf = realloc(buf, wanted);
		if (buf == NULL)
			panic("out of memory constructing a string");
		memset(buf + len, 'x', wanted - len);
		len = wanted;
	}
	return buf;
}

static void print_stats(Tbuf *tbuf) {
	struct tbuf_stat stat;
	
	tbuf_get_stats(&stat, tbuf);
	printf("%lu %lu %lu %lu %lu %lu\n",
		(unsigned long) stat.tbuf_length,
		(unsigned long) stat.tbuf_height,
		(unsigned long) stat.tbuf_nodes,
		(unsigned long) stat.tbuf_chunks,
		(unsigned long) stat.tbuf_memory_total,
		(unsigned long) stat.tbuf_nodes_with_few_kids);
}

static void test_single_inserts(int number) {
	Tbuf *tbuf, *tbuf_char, *tbuf_new;
	
	tbuf = x_tbuf_create("", 0);
	while (number-- > 0) {
		tbuf_char = x_tbuf_create("x", 1);
		tbuf_new = x_tbuf_cat(tbuf, tbuf_char);
		print_stats(tbuf_new);
		tbuf_destroy(tbuf_char);
		tbuf_destroy(tbuf);
		tbuf = tbuf_new;
	}
}

int main(int argc, char **argv) {
#if 1
	test_single_inserts(10000);
	return 0;
#else
	Tbuf *tbuf, *tbuf_new, *tbuf1, *tbuf2, *tbuf3, *tbuf4;
	unsigned long offset, len, new_len, lineno;
	FILE *f;
	
	if (argc == 1)
		f = stdin;
	else {
		f = fopen(argv[1], "r");
		if (f == NULL) {
			perror("fopen");
			exit(EXIT_FAILURE);
		}
	}

	tbuf = x_tbuf_create("", 0);
	lineno = 0;
	while (fscanf(f, "%lu%lu%lu", &offset, &len, &new_len) == 3) {
		++lineno;
#if 0
		fprintf(stderr, "line %lu %lu %lu %lu\n", lineno, offset, len, new_len);
#endif
		tbuf1 = x_tbuf_copy(tbuf, 0, offset);
		tbuf2 = x_tbuf_create(x_make_text(new_len), new_len);
		tbuf3 = x_tbuf_copy(tbuf, offset + len,
				tbuf_length(tbuf) - offset - len);
		tbuf4 = x_tbuf_cat(tbuf1, tbuf2);
		tbuf_new = x_tbuf_cat(tbuf4, tbuf3);
		tbuf_destroy(tbuf1);
		tbuf_destroy(tbuf2);
		tbuf_destroy(tbuf3);
		tbuf_destroy(tbuf4);
		print_stats(tbuf_new);
		if (tbuf != NULL)
			tbuf_destroy(tbuf);
		tbuf = tbuf_new;
	}
	
	fclose(f);
	return 0;
#endif
}
