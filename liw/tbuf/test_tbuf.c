/*
 * test_tbuf.c -- test program for tbuf
 *
 * Part of Publib, see man page for more information.
 * "@(#)publib-tbuf:$Id: test_tbuf.c,v 1.2 1998/06/24 10:04:20 liw Exp $"
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

int main(void) {
    Tbuf *tbuf, *tbuf2, *tbuf3;
    char str[] = "abcdefghijklmnopqrstuvwxyz";
    char buf[10240];
    size_t len, len2, max;

    max = strlen(str);
    for (len = 0; len <= max; ++len) {
        tbuf = x_tbuf_create(str, len);
	memset(buf, 0, sizeof(buf));
	tbuf_chars(buf, tbuf, 0, len);
	printf("buf =<%s> (len=%u)\n", buf, len);

	tbuf2 = x_tbuf_copy(tbuf, 0, len);
	tbuf_destroy(tbuf);
	memset(buf, 0, sizeof(buf));
	tbuf_chars(buf, tbuf2, 0, len);
	printf("buf2=<%s> (len=%u)\n", buf, len);

	tbuf_destroy(tbuf2);
    }

    for (len = 0; len <= max; ++len) {
        tbuf = x_tbuf_create(str, len);
	for (len2 = 0; len2 <= max; ++len2) {
	        tbuf2 = x_tbuf_create(str, len2);
		tbuf3 = x_tbuf_cat(tbuf, tbuf2);
		memset(buf, 0, sizeof(buf));
		tbuf_chars(buf, tbuf3, 0, len + len2);
		printf("buf=<%s> (len=%u len2=%u)\n", buf, len, len2);
		tbuf_destroy(tbuf2);
		tbuf_destroy(tbuf3);
	}
    }

    return 0;
}
