/*
 * sbuf_aux.c -- auxiliary routines for text editors
 *
 * Part of Publib.  See manpage for more information.
 * "@(#)publib-sbuf:$Id: sbuf_aux.c,v 1.14 1996/12/18 15:52:26 liw Exp $"
 */


#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "publib/sbuf.h"
#include "publib/errormsg.h"


/*
 * This macro is used by sbuf_bow and sbuf_eow to determine if a
 * character is a `letter', i.e., if it can belong to a word.
 */
#define letter(c) ((c) == '_' || isalnum(c))



/*
 * Local function prototypes.
 */
static void put_into_cache(Sbuf *, long, long);
static int find_nearest(Sbuf *, long);
static long compute_lineno(Sbuf *, long, long, long);
static long find_pair(const unsigned char *, long, long);

#ifdef NDEBUG
#define check_pos_cache(x) ((void) 0)
#else
static void check_pos_cache(Sbuf *);
#endif



/*
 * Return position before first character on line containing pos.
 */
long sbuf_boln(Sbuf *buf, long pos) {
	while (pos > 0 && sbuf_charat(buf, pos-1) != '\n')
		--pos;
	return pos;
}


/*
 * Return position after last character (including newline) on line
 * containing pos.
 */
long sbuf_eoln(Sbuf *buf, long pos) {
        int c;

        while ((c = sbuf_charat(buf, pos)) != '\n' && c != EOF)
                ++pos; 
        if (c == EOF)
                return pos;
        return pos+1;
}


/*
 * Return position before first character of the word that contains
 * pos.  If pos is not within a word, return pos.
 */
long sbuf_bow(Sbuf *buf, long pos) {
        int c;

	c = sbuf_charat(buf, pos);
        if (!letter(c))
                return pos;
        while (pos > 0 && (c = sbuf_charat(buf, pos-1), letter(c)))
                --pos;
        return pos;
}



/*
 * Return position after last character of the word that contains
 * pos.  If pos is not within a word, return pos+1 (or pos, if at
 * EOF).
 */
long sbuf_eow(Sbuf *buf, long pos) {
        int c;

	c = sbuf_charat(buf, pos);
        if (!letter(c)) {
                if (c != EOF)
                        ++pos;
                return pos;
        }
        while (letter(c)) {
                ++pos;
		c = sbuf_charat(buf, pos);
	}
        return pos;
}


/*
 * Return line number of character after position pos.  The first line is line 0.
 */
long sbuf_lineno(Sbuf *buf, long pos) {
	int i;
	long lineno;
	struct __sbuf_pos_cache *cache;

	check_pos_cache(buf);

	cache = buf->pc.data;

	i = find_nearest(buf, pos);
	if (i == -1)
		lineno = compute_lineno(buf, pos, 0, 0);
	else
		lineno = compute_lineno(buf, pos, 
				cache[i].pos, cache[i].lineno);

	if (i == -1 || cache[i].pos != pos)
		put_into_cache(buf, pos, lineno);

	check_pos_cache(buf);
	return lineno;
}



/*
 * Return position before first character of line number lineno 
 * (first line is 0).  FIXME: this should use the poscache to
 * find the closest position.
 */
long sbuf_linepos(Sbuf *buf, long lineno) {
	long gaplen;
	char *p, *q;
	
	p = buf->block;
	q = buf->block + buf->gappos;
	while (p < q && lineno > 0) {
		if (*p == '\n')
			--lineno;
		++p;
	}
	
	if (lineno == 0)
		return p - buf->block;

	gaplen = buf->alloc - buf->len;
	p = buf->block + buf->gappos + gaplen;
	q = buf->block + buf->len + gaplen;
	while (p < q && lineno > 0) {
		if (*p == '\n')
			--lineno;
		++p;
	}

	return p - buf->block - gaplen;
}



/*
 * Function:	sbuf_colno
 * Purpose:	Return column number of a text position.
 * Arguments:	buf	the buffer
 *		pos	the position
 *		tab	tab distance
 * Return:	The column number.
 * Note:	The first column is number 0.
 */
long sbuf_colno(Sbuf *buf, long pos, long tab) {
	long p, col;

	p = sbuf_boln(buf, pos);
	col = 0;
	while (p < pos) {
		if (sbuf_charat(buf, p) == '\t')
			col += tab - (col % tab);
		else
			++col;
		++p;
	}
	return col;
}



/*
 * Function:	sbuf_colpos
 * Purpose:	Return the position of the character in a given column.
 * Arguments:	buf	the buffer
 *		pos	the beginning of the line
 *		col	the column
 *		tab	tab distance
 * Return:	The text position of the character that occupies the given
 *		column.
 * Note:	A tab occupies several character positions.  If the line
 *		is too short, the position of the newline or EOF that
 *		ends the line is returned.
 */
long sbuf_colpos(Sbuf *buf, long pos, long col, long tab) {
	long i;

	i = 0;
	while (i < col) {
		switch (sbuf_charat(buf, pos)) {
		case '\n':
		case EOF:
			return pos;
		case '\t':
			i += tab - (i % tab);
			if (i > col)
				return pos;
			break;
		default:
			++i;
			break;
		}
		++pos;
	}
	return pos;

}


/*
 * Function:	sbuf_clear_pos_cache
 * Purpose:	Clear the poscache starting at position pos.
 */
void sbuf_clear_pos_cache(Sbuf *buf, long pos) {
	struct __sbuf_pos_cache *cache;
	
	check_pos_cache(buf);
	cache = buf->pc.data;
	while (buf->pc.used > 0 && cache[buf->pc.used-1].pos >= pos)
		--buf->pc.used;
	check_pos_cache(buf);
}



/*
 * Function:	sbuf_adjust_pos_cache
 * Purpose:	Adjust the poscache.
 * Description:	The buffer has been modified by replacing pos..pos+oldlen-1
 *		with new text that is newlen chars long.  The range now
 *		has n more (n > 0) or fewer (n < 0) newlines.  The cache
 *		is adjusted so that each (p,lineno) pair is deleted or
 *		updated as necessary to bring it up to date to the current
 *		situation: pairs in the old range are deleted and those after
 *		it are moved and the line number adjusted.
 */
void sbuf_adjust_pos_cache(Sbuf *buf, long pos, long oldlen, long newlen,
long n) {
	int i, j;
	struct __sbuf_pos_cache *cache;

	check_pos_cache(buf);
	
	cache = buf->pc.data;

	/* fixme: bsearch should be used */
	for (i = 0; i < buf->pc.used && cache[i].pos < pos; ++i)
		continue;

	j = i;
	for (; i < buf->pc.used && cache[i].pos < pos + oldlen; ++i)
		continue;

	memmove(cache + j, cache + i, (buf->pc.used - i) * sizeof(cache[0]));
	buf->pc.used -= i - j;

	for (; j < buf->pc.used; ++j) {
		cache[j].pos += newlen - oldlen;
		cache[j].lineno += n;
	}

	check_pos_cache(buf);
}



/*
 * Function:	sbuf_find_pair
 * Purpose:	Find pair to character at a given position.
 */
long sbuf_find_pair(Sbuf *buf, long pos) {
	unsigned char *str;

	str = (unsigned char *) sbuf_lock(buf);
	if (str == NULL)
		return -1;
	pos = find_pair(str, sbuf_length(buf), pos);
	sbuf_unlock(buf);
	return pos;
}



/*
 * Write position cache statistics to file.
 */
void sbuf_cache_stats(Sbuf *buf, FILE *f) {
	fprintf(f, "Sbuf position cache statistics (%p):\n", (void *) buf);
	fprintf(f, "   hits:     %6ld\n", buf->poshits);
	fprintf(f, "   misses:   %6ld\n", buf->posmisses);
	fprintf(f, "   avg miss: %8.1f\n", buf->posmissavg);
}


/*
 * Is a position inside a mark?
 */
int sbuf_pos_inside_mark(Sbufmark *mark, long pos) {
	long begin, end, tabsize, poscol;
	
	begin = sbuf_mark_begin(mark);
	if (pos < begin)
		return 0;
	end = sbuf_mark_end(mark);
	if (pos >= end)
		return 0;
	if (!sbuf_mark_is_columnar(mark))
		return 1;

	tabsize = 8;	
	poscol = sbuf_colno(mark->buf, pos, tabsize);
	if (poscol < sbuf_colno(mark->buf, begin, tabsize))
		return 0;
	return poscol < sbuf_colno(mark->buf, end, tabsize);
}



/**************************************************************************
 * Local functions follow.
 */


static int find_nearest(Sbuf *buf, long pos) {
	struct __sbuf_pos_cache *cache;
	int i;

	check_pos_cache(buf);

	cache = buf->pc.data;
	
	if (buf->pc.used == 0)
		return -1;
	if (buf->pc.used == 1 || pos <= cache[0].pos)
		return 0;
	if (pos >= cache[buf->pc.used-1].pos)
		return buf->pc.used-1;

	for (i = 0; pos > cache[i+1].pos; ++i)
		continue;

	check_pos_cache(buf);

	if (i == buf->pc.used-1)
		return i;
	else if (pos - cache[i].pos < cache[i+1].pos - pos)
		return i;
	else
		return i+1;
}


static long compute_lineno(Sbuf *buf, long pos, long known_pos, long lineno) {
	long orig_pos;

	orig_pos = known_pos;

	while (known_pos < pos) {
		if (sbuf_charat(buf, known_pos) == '\n')
			++lineno;
		++known_pos;
	}

	while (known_pos > pos) {
		--known_pos;
		if (sbuf_charat(buf, known_pos) == '\n')
			--lineno;
	}
	
	if (known_pos == orig_pos)
		++buf->poshits;
	else {
		buf->posmissavg = 
		    (buf->posmisses * buf->posmissavg + 
		        labs(known_pos - orig_pos)) / (buf->posmisses + 1);
		++buf->posmisses;
	}

	return lineno;
}



static void put_into_cache(Sbuf *buf, long pos, long lineno) {
	struct __sbuf_pos_cache *cache;
	long dist, mindist;
	int i, min;
	
	check_pos_cache(buf);
	
	if (buf->pc.used < SBUF_POS_CACHE_MAX && buf->pc.used == buf->pc.alloc)
		(void) dynarr_resize(&buf->pc, buf->pc.used + 1);
	
	cache = buf->pc.data;

	assert(buf->pc.alloc >= SBUF_POS_CACHE_MIN);
	if (buf->pc.used >= buf->pc.alloc - 1) {
		min = 2;
		mindist = cache[min].pos - cache[min-2].pos;
		for (i = 3; i < buf->pc.used; ++i) {
			dist = cache[i].pos - cache[i-2].pos;
			if (dist < mindist) {
				min = i;
				mindist = dist;
			}
		}
		for (i = min; i < buf->pc.used; ++i)
			cache[i-1] = cache[i];
		--buf->pc.used;
	}

	for (i = buf->pc.used; i > 0; --i) {
		if (pos > cache[i-1].pos)
			break;
		cache[i] = cache[i-1];
	}
	cache[i].pos = pos;
	cache[i].lineno = lineno;
	++buf->pc.used;

	check_pos_cache(buf);
}



/*
 * Function:	find_pair
 * Purpose:	Given a buffer, find the pair of a character
 * Arguments:	str	the string of the buffer
 *		len	length of buffer
 *		pos	the position of the character
 * Return:	-1 for no pair found, or position in buffer of pair.
 */
static long find_pair(const unsigned char *str, long len, long pos) {
	static unsigned char pairstr[] = "(){}[]";
	static int pair[UCHAR_MAX + 1];
	static int dir[UCHAR_MAX + 1];
	static int init = 0;
	int c;

	if (pos < 0 || pos >= len)
		return -1;

	if (!init) {
		for (c = 0; c < UCHAR_MAX + 1; ++c) {
			pair[c] = EOF;
			dir[c] = 0;
		}

		assert(strlen(pairstr) % 2 == 0);
		for (c = 0; pairstr[c] != '\0'; c += 2) {
			pair[pairstr[c]] = pairstr[c+1];
			pair[pairstr[c+1]] =pairstr[c];
			dir[pairstr[c]] = 1;
			dir[pairstr[c+1]] = -1;
		}

		init = 1;
	}

	c = str[pos];
	if (pair[c] == EOF)
		return -1;

	for (;;) {
		assert(dir[c] != 0);
		do {
			pos += dir[c];
			if (pos < 0 || pos >= len)
				return -1;
		} while (pair[str[pos]] == EOF);
	
		if (pair[str[pos]] == c)
			return pos;
		if (dir[str[pos]] != dir[c])
			return -1;
		pos = find_pair(str, len, pos);
		if (pos < 0 || pos >= len)
			return -1;
	}
}



/*
 * Function:	check_pos_cache
 * Purpose:	Check that the pos cache is OK.
 */
#ifndef NDEBUG
static void check_pos_cache(Sbuf *buf) {
	int i;
	struct __sbuf_pos_cache *cache;

	assert(buf->pc.alloc >= SBUF_POS_CACHE_MIN);
	assert(buf->pc.alloc <= SBUF_POS_CACHE_MAX);
	
	if (buf->pc.used == 0)
		return;
	
	cache = buf->pc.data;
	assert(buf->pc.used > 0);
	assert(cache[0].pos >= 0);
	assert(cache[0].lineno >= 0);
	for (i = 1; i < buf->pc.used; ++i) {
		assert(cache[i].pos >= 0);
		assert(cache[i].lineno >= 0);
		assert(cache[i].pos > cache[i-1].pos);
		assert(cache[i].lineno >= cache[i-1].lineno);
	}
}
#endif
