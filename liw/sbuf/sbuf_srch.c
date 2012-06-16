/*
 * sbuf_srch.c -- implement searching routines for text editor buffer
 *
 * Part of Publib.  See manpage for more information.
 * "@(#)publib-sbuf:$Id: sbuf_srch.c,v 1.3 1996/03/16 22:19:05 liw Exp $"
 */

#include <ctype.h>
#include "publib/sbuf.h"

typedef int func(Sbuf *, long, const char *, size_t);

static func is_match;
static func is_match_nocase;

static int srch_forward(Sbufmark *, Sbufmark *, const char *, size_t, func *);
static int srch_backward(Sbufmark *, Sbufmark *, const char *, size_t, func *);

int sbuf_search(Sbufmark *m, Sbufmark *inside, const char *str, size_t len, 
unsigned long opts) {
	switch (opts) {
	case SBUF_BACKWARD | SBUF_IGNORE_CASE:
		return srch_backward(m, inside, str, len, is_match_nocase);
	case SBUF_BACKWARD:
		return srch_backward(m, inside, str, len, is_match);
	case SBUF_IGNORE_CASE:
		return srch_forward(m, inside, str, len, is_match_nocase);
	case 0:
		return srch_forward(m, inside, str, len, is_match);
	}
	return -1;	/* if options are invalid... */
}



static int srch_forward(Sbufmark *m, Sbufmark *inside, const char *str, 
size_t len, func *match) {
	Sbuf *buf;
	long begin, end, pos;

	buf = inside->buf;
	begin = sbuf_mark_begin(inside);
	end = sbuf_mark_end(inside);

	for (pos = begin; pos + len <= end; ++pos) {
		if (match(buf, pos, str, len)) {
			sbuf_remark(m, pos, len);
			return 0;
		}
	}
	return -1;
}


static int srch_backward(Sbufmark *m, Sbufmark *inside, const char *str, 
size_t len, func *match) {
	Sbuf *buf;
	long begin, end, pos;

	buf = inside->buf;
	begin = sbuf_mark_begin(inside);
	end = sbuf_mark_end(inside);

	for (pos = end - len; pos >= begin; --pos) {
		if (match(buf, pos, str, len)) {
			sbuf_remark(m, pos, len);
			return 0;
		}
	}
	return -1;
}


static int is_match(Sbuf *buf, long pos, const char *str, size_t len) {
	long buflen;

	buflen = sbuf_length(buf);
	while (len > 0 && pos < buflen && *str == sbuf_charat(buf, pos)) {
		++pos;
		++str;
		--len;
	}
	return len == 0;
}


static int is_match_nocase(Sbuf *buf, long pos, const char *str, size_t len) {
	long buflen;
	int c;

	buflen = sbuf_length(buf);
	while (len > 0 && pos < buflen) {
		c = sbuf_charat(buf, pos);
		if (toupper(*str) != toupper(c))
			break;
		++pos;
		++str;
		--len;
	}
	return len == 0;
}
