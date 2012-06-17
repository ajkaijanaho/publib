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
 * test_tbuf.c -- test program for tbuf
 *
 * Part of Publib, see man page for more information.
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
	printf("buf =<%s> (len=%zu)\n", buf, len);

	tbuf2 = x_tbuf_copy(tbuf, 0, len);
	tbuf_destroy(tbuf);
	memset(buf, 0, sizeof(buf));
	tbuf_chars(buf, tbuf2, 0, len);
	printf("buf2=<%s> (len=%zu)\n", buf, len);

	tbuf_destroy(tbuf2);
    }

    for (len = 0; len <= max; ++len) {
        tbuf = x_tbuf_create(str, len);
	for (len2 = 0; len2 <= max; ++len2) {
	        tbuf2 = x_tbuf_create(str, len2);
		tbuf3 = x_tbuf_cat(tbuf, tbuf2);
		memset(buf, 0, sizeof(buf));
		tbuf_chars(buf, tbuf3, 0, len + len2);
		printf("buf=<%s> (len=%zu len2=%zu)\n", buf, len, len2);
		tbuf_destroy(tbuf2);
		tbuf_destroy(tbuf3);
	}
    }

    return 0;
}
