/*
 * sbuf_srch.c -- implement searching routines for text editor buffer
 *
 * Part of Publib.  See manpage for more information.
 * "@(#)publib-sbuf:$Id: sbuf_srch.c,v 1.4 1996/11/04 02:21:22 liw Exp $"
 */

#include <stdio.h>
#include <ctype.h>
#include <regex.h>
#include "publib/sbuf.h"

typedef long func(Sbuf *, long, const char *, size_t);

static func is_match;
static func is_match_nocase;

static int srch_forward(Sbufmark *, Sbufmark *, const char *, size_t, func *);
static int srch_backward(Sbufmark *, Sbufmark *, const char *, size_t, func *);
static int srch_forward_regex(Sbufmark *, Sbufmark *, const char *, size_t, unsigned long);
static int srch_backward_regex(Sbufmark *, Sbufmark *, const char *, size_t, unsigned long);

int sbuf_search(Sbufmark *m, Sbufmark *inside, const char *str, size_t len, 
unsigned long opts) {
	if (opts & SBUF_REGEX) {
		if (opts & SBUF_BACKWARD)
			return srch_backward_regex(m, inside, str, len, opts);
		return srch_forward_regex(m, inside, str, len, opts);
	}

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
	long begin, end, pos, match_len;

	buf = inside->buf;
	begin = sbuf_mark_begin(inside);
	end = sbuf_mark_end(inside);

	for (pos = begin; pos + len <= end; ++pos) {
		match_len = match(buf, pos, str, len);
		if (match_len >= 0) {
			sbuf_remark(m, pos, match_len);
			return 0;
		}
	}
	return -1;
}


static int srch_backward(Sbufmark *m, Sbufmark *inside, const char *str, 
size_t len, func *match) {
	Sbuf *buf;
	long begin, end, pos, match_len;

	buf = inside->buf;
	begin = sbuf_mark_begin(inside);
	end = sbuf_mark_end(inside);

	for (pos = end - len; pos >= begin; --pos) {
		match_len = match(buf, pos, str, len);
		if (match_len >= 0) {
			sbuf_remark(m, pos, match_len);
			return 0;
		}
	}
	return -1;
}


static int srch_forward_regex(Sbufmark *m, Sbufmark *inside, const char *str,
size_t len, unsigned long opts) {
	Sbuf *buf;
	regex_t preg;
	regmatch_t pmatch[1];
	char *text;
	long ret, begin, end;
	int n;
	int mask;
	
	buf = inside->buf;
	text = sbuf_lock(buf);
	if (text == NULL)
		return -1;
		
	begin = sbuf_mark_begin(inside);
	end = sbuf_mark_end(inside);
	
	mask = REG_EXTENDED | REG_NEWLINE;
	if (opts & SBUF_IGNORE_CASE)
		mask |= REG_ICASE;

	/* fixme: str need not be zero delimited */
	ret = -1;
	if (regcomp(&preg, str, mask) == 0) {
		n = regexec(&preg, text + begin, 1, pmatch, 0);
		if (n == 0 &&
		    pmatch[0].rm_so != -1 && 
		    pmatch[0].rm_eo + begin <= end) {
			ret = 0;
			sbuf_remark(m, begin + pmatch[0].rm_so,
				pmatch[0].rm_eo - pmatch[0].rm_so);
		}
		regfree(&preg);
	}

	sbuf_unlock(buf);
	return ret;
}


static int srch_backward_regex(Sbufmark *m, Sbufmark *inside, const char *str,
size_t len, unsigned long opts) {
	long ibegin, iend;
	long mbegin, mlen;

	if (srch_forward_regex(m, inside, str, len, opts) == -1)
		return -1;

	ibegin = sbuf_mark_begin(inside);
	iend = sbuf_mark_end(inside);

	do {
		mbegin = sbuf_mark_begin(m);
		mlen = sbuf_mark_length(m);
		if (mbegin+1 >= iend)
			break;
		sbuf_remark(inside, mbegin+1, iend - (mbegin+1));
	} while (srch_forward_regex(m, inside, str, len, opts) == 0);

	sbuf_remark(inside, ibegin, iend - ibegin);
	sbuf_remark(m, mbegin, mlen);
	return 0;
}


static long is_match(Sbuf *buf, long pos, const char *str, size_t len) {
	long buflen, match_len;

	buflen = sbuf_length(buf);
	match_len = len;
	while (len > 0 && pos < buflen && *str == sbuf_charat(buf, pos)) {
		++pos;
		++str;
		--len;
	}
	if (len == 0)
		return match_len;
	return -1;
}


static long is_match_nocase(Sbuf *buf, long pos, const char *str, size_t len) {
	long buflen, match_len;
	int c;

	buflen = sbuf_length(buf);
	match_len = len;
	while (len > 0 && pos < buflen) {
		c = sbuf_charat(buf, pos);
		if (toupper(*str) != toupper(c))
			break;
		++pos;
		++str;
		--len;
	}
	if (len == 0)
		return match_len;
	return -1;
}
