/*
 * sbuf.c -- simple text editor buffer routines
 *
 * Part of Publib.  See manpage for more information.
 * "@(#)publib-sbuf:$Id: sbuf.c,v 1.15 1996/12/27 22:48:33 liw Exp $"
 */


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "publib/sbuf.h"
#include "publib/errormsg.h"


/* Return length of gap */
#define gaplen(buf)	(buf->alloc - buf->len)



/*
 * Prototypes for local functions.
 */
static int replace_mark_part(Sbuf *, long, long, const char *, long, long);
static void adjust_marks(Sbuf *, long, long, long);
static int remove_columnar_mark(Sbufmark *);
static long count_chars(const char *, int, long);
static int insert_columnar_text(Sbuf *, Sbufmark *, const char *, long, long);
static int strchange(Sbuf *, long, long, const char *, long);



/* Create a buffer. */
Sbuf *sbuf_create(void) {
	Sbuf *buf;

	buf = malloc(sizeof(Sbuf));
	if (buf == NULL) {
		__publib_error("malloc failed");
		return NULL;
	}

	buf->block = NULL;
	buf->name = NULL;
	buf->flags = 0;
	buf->gappos = 0;
	buf->alloc = 0;
	buf->len = 0;
	buf->marks = NULL;
	buf->markalloc = 0;

	dynarr_init(&buf->pc, sizeof(struct __sbuf_pos_cache));
	if (dynarr_resize(&buf->pc, SBUF_POS_CACHE_MIN) == -1) {
		dynarr_free(&buf->pc);
		free(buf);
		return NULL;
	}
	buf->poshits = 0;
	buf->posmisses = 0;
	buf->posmissavg = 0;
	
	buf->log_head = NULL;
	buf->log_tail = NULL;

	sbuf_validate(buf);
	
	buf->aux = sbuf_mark(buf, 0, 0);
	if (buf == NULL) {
		sbuf_destroy(buf);
		return NULL;
	}

	return buf;
}



/* Destroy a buffer */
void sbuf_destroy(Sbuf *buf) {
	sbuf_validate(buf);

	free(buf->block);
	free(buf->marks);
	free(buf);
}



/* Check that all the data structures for the buffer are OK.  Abort program
   if they aren't.  */
void (sbuf_validate)(Sbuf *buf) {
	struct __sbufmark *m;
	size_t i;

	assert(buf != NULL);
	assert(buf->len <= buf->alloc);
	assert(buf->block != NULL || buf->alloc == 0);
	assert(buf->gappos <= buf->alloc);
	assert(buf->len <= buf->alloc);

	assert(buf->marks != NULL || buf->markalloc == 0);
	for (i = 0; i < buf->markalloc; ++i) {
		m = buf->marks + i;
		if (m->inuse)
			assert(m->begin + m->len <= buf->len);
	}
}



/* Move gap before the pos'th character in text, and make sure it is at
   least len characters. */
int sbuf_movegap(Sbuf *buf, size_t pos, size_t len) {
	char *p;

	sbuf_validate(buf);
	assert(pos <= buf->len);
	assert(!buf->locked);

	if (gaplen(buf) < len || buf->block == NULL) {
		if (buf->block != NULL && sbuf_movegap(buf, buf->len, 0) < 0) {
			__publib_error("recursive sbuf_failed (can't happen)");
			return -1;
		}
		p = realloc(buf->block, buf->alloc + len + 4);
		if (p == NULL) {
			__publib_error("realloc failed");
			return -1;
		}
		buf->block = p;
		buf->alloc += len;
	}

	assert(buf->block != NULL);
        if (pos < buf->gappos)
               	memmove(buf->block + pos + gaplen(buf),
                       	buf->block + pos,
                       	buf->gappos - pos);
        else
               	memmove(buf->block + buf->gappos,
                       	buf->block + buf->gappos + gaplen(buf),
                       	pos - buf->gappos);
        buf->gappos = pos;

	sbuf_validate(buf);

	return 0;
}


char *sbuf_get_name(Sbuf *buf) {
	return buf->name;
}


int sbuf_set_name(Sbuf *buf, const char *newname) {
	newname = strdup(newname);
	if (newname == NULL)
		return -1;
	free(buf->name);
	buf->name = (char *) newname;
	return 0;
}


unsigned long sbuf_get_flags(Sbuf *buf) {
	return buf->flags;
}


int sbuf_has_flags(Sbuf *buf, unsigned long mask) {
	return (buf->flags & mask) == mask;
}


void sbuf_set_all_flags(Sbuf *buf, unsigned long flags) {
	buf->flags = flags;
}


void sbuf_set_flags(Sbuf *buf, unsigned long mask) {
	buf->flags |= mask;
}


void sbuf_clear_flags(Sbuf *buf, unsigned long mask) {
	buf->flags &= ~mask;
}


/*
 * Function:	sbuf_is_dirty
 * Purpose:	Return dirty bit for buffer.
 */
int sbuf_is_dirty(Sbuf *buf) {
	return sbuf_has_flags(buf, SBUF_DIRTY_FLAG);
}



/*
 * Function:	sbuf_set_dirty
 * Purpose:	Set dirty bit for buffer.
 */
void sbuf_set_dirty(Sbuf *buf, int dirty) {
	if (dirty)
		sbuf_set_flags(buf, SBUF_DIRTY_FLAG);
	else
		sbuf_clear_flags(buf, SBUF_DIRTY_FLAG);
}



/* Lock buffer against changes and return pointer to a contiguous block
   of memory holding the buffer contents (which may be modified except
   its length may not be modified).  */
char *sbuf_lock(Sbuf *buf) {
	sbuf_validate(buf);
	assert(!sbuf_has_flags(buf, SBUF_LOCKED_FLAG));

	if (sbuf_movegap(buf, buf->len, 0) == -1) {
		__publib_error("sbuf_movegap failed");
		return NULL;
	}
	sbuf_set_flags(buf, SBUF_LOCKED_FLAG);
	return buf->block;
}



/* Release a lock on a buffer.  The user may not assume that the pointer
   returned by sbuf_lock is usable any longer.  */
void sbuf_unlock(Sbuf *buf) {
	sbuf_validate(buf);
	assert(sbuf_has_flags(buf, SBUF_LOCKED_FLAG));

	sbuf_clear_flags(buf, SBUF_LOCKED_FLAG);
}



/* Return length (== number of character in the buffer). */
long (sbuf_length)(Sbuf *buf) {
	sbuf_validate(buf);
	return buf->len;
}



/* Return the character at position pos.  Return EOF if just after last
   character.  */
int (sbuf_charat)(Sbuf *buf, long pos) {
	sbuf_validate(buf);
	assert(pos >= 0);
	assert(pos <= buf->len);
	assert(!buf->locked);

	if (pos == buf->len)
		return EOF;

	assert(buf->block != NULL);
	if (pos < buf->gappos)
		return (unsigned char) buf->block[pos];
	return (unsigned char) buf->block[pos + gaplen(buf)];
}



/* Create a mark. */
Sbufmark *sbuf_mark(Sbuf *buf, long pos, long len) {
	struct __sbufmark *p;
	Sbufmark *q;
	size_t i;

	sbuf_validate(buf);
	assert(pos >= 0);
	assert(len >= 0);
	assert(pos + len <= buf->len);

	p = NULL;
	for (i = 0; i < buf->markalloc; ++i) {
		if (!buf->marks[i].inuse) {
			p = buf->marks + i;
			break;
		}
	}
	if (p == NULL) {
		i = buf->markalloc + 8;
		p = realloc(buf->marks, i * sizeof(struct __sbufmark));
		if (p == NULL) {
			__publib_error("realloc failed");
			return NULL;
		}
		buf->marks = p;
		p = buf->marks + buf->markalloc;
		while (buf->markalloc < i)
			buf->marks[buf->markalloc++].inuse = 0;
		buf->markalloc = i;
	}

	q = malloc(sizeof(Sbufmark));
	if (q == NULL) {
		__publib_error("malloc failed");
		return NULL;
	}

	p->inuse = 1;
	p->begin = pos;
	p->len = len;
	p->dirty = 0;
	p->columnar = 0;
	p->code = 0;
	p->handle = q;

	q->buf = buf;
	q->mark = p - buf->marks;

	sbuf_validate(buf);
	sbuf_validate_mark(q);

	return q;
}



/* Destroy a mark.  */
void sbuf_unmark(Sbufmark *mark) {
	sbuf_validate_mark(mark);

	mark->buf->marks[mark->mark].inuse = 0;
}



/* Check that a mark's data structures are ok.  */
void (sbuf_validate_mark)(Sbufmark *mark) {
	assert(mark != NULL);
	sbuf_validate(mark->buf);
	assert(mark->mark < mark->buf->markalloc);
	assert(mark->buf->marks[mark->mark].inuse);
}



/* Move an existing mark. */
void sbuf_remark(Sbufmark *mark, long pos, long len) {
	sbuf_validate_mark(mark);
	assert(pos >= 0);
	assert(len >= 0);
	assert(pos + len <= mark->buf->len);

	mark->buf->marks[mark->mark].begin = pos;
	mark->buf->marks[mark->mark].len = len;
	mark->buf->marks[mark->mark].dirty = 0;
}



/* Return starting position of mark. */
long (sbuf_mark_begin)(Sbufmark *mark) {
	sbuf_validate_mark(mark);
	return mark->buf->marks[mark->mark].begin;
}



/* Return end position of mark.  */
long (sbuf_mark_end)(Sbufmark *mark) {
	sbuf_validate_mark(mark);
	return mark->buf->marks[mark->mark].begin +
               mark->buf->marks[mark->mark].len;
}



/* Return length of mark.  */
long (sbuf_mark_length)(Sbufmark *mark) {
	struct __sbufmark *m;
	long len, tabsize, startcol, endcol, col, pos;
	int c;

	sbuf_validate_mark(mark);

	m = mark->buf->marks + mark->mark;

	if (!m->columnar)
		return m->len;

	len = 0;
	tabsize = 8;
	startcol = sbuf_colno(mark->buf, m->begin, tabsize);
	endcol = sbuf_colno(mark->buf, m->begin + m->len, tabsize);
	col = startcol;
	c = EOF;
	for (pos = 0; pos < m->len; ++pos) {
		c = sbuf_charat(mark->buf, m->begin + pos);
		if (c == '\n') {
			++len;
			col = 0;
		} else {
			if (col >= startcol && col < endcol)
				++len;
			if (c == '\t')
				col += tabsize - (col % tabsize);
			else
				++col;
		}
	}

	return len;
}



/* Return dirty bit.  */
int (sbuf_mark_is_dirty)(Sbufmark *mark) {
	sbuf_validate_mark(mark);
	return mark->buf->marks[mark->mark].dirty;
}



/* Set dirty bit.  */
void (sbuf_mark_set_dirty)(Sbufmark *mark, int dirty) {
	sbuf_validate_mark(mark);
	mark->buf->marks[mark->mark].dirty = dirty;
}



/* Return columnar mode.  */
int (sbuf_mark_is_columnar)(Sbufmark *mark) {
	sbuf_validate_mark(mark);
	return mark->buf->marks[mark->mark].columnar;
}



/* Set columnar mode.  */
void (sbuf_mark_set_columnar)(Sbufmark *mark, int mode) {
	sbuf_validate_mark(mark);
	mark->buf->marks[mark->mark].columnar = !!mode;
}


void sbuf_set_mark_code(Sbufmark *mark, int code) {
	mark->buf->marks[mark->mark].code = code;
}

int sbuf_get_mark_code(Sbufmark *mark) {
	return mark->buf->marks[mark->mark].code;
}

Sbufmark *sbuf_find_mark_by_code(Sbuf *buf, int code) {
	int i;
	struct __sbufmark *m;

	if (code == 0)
		return NULL;
	for (i = 0; i < buf->markalloc; ++i) {
		m = buf->marks + i;
		if (m->inuse && m->code == code)
			return m->handle;
	}
	return NULL;
}



/* Copy marked text to a string.  */
void sbuf_strat(char *str, Sbufmark *mark) {
	struct __sbufmark *m;
	char *p;
	const int tabsize = 8;
	long startcol, endcol, col, pos;

	assert(str != NULL);
	sbuf_validate_mark(mark);
	
	m = mark->buf->marks + mark->mark;
	p = sbuf_lock(mark->buf);

	if (m->columnar) {
		startcol = sbuf_colno(mark->buf, m->begin, tabsize);
		endcol = sbuf_colno(mark->buf, m->begin + m->len, tabsize);
		col = startcol;
		for (pos = m->len, p += m->begin; pos > 0; --pos, ++p) {
			if (*p == '\n') {
				*str++ = *p;
				col = 0;
			} else {
				if (col >= startcol && col < endcol)
					*str++ = *p;
				if (*p == '\t')
					col += tabsize - (col % tabsize);
				else
					++col;
			}
		}
		*str = '\0';
	} else {
		if (m->len > 0)
			memcpy(str, p + m->begin, m->len);
		str[m->len] = '\0';
	}

	sbuf_unlock(mark->buf);
}



/* Replace the contents of a mark with a string.  */
int sbuf_strchange(Sbufmark *mark, const char *str, size_t len) {
	struct __sbufmark *m;

	sbuf_validate_mark(mark);
	assert(str != NULL);
	assert(!mark->buf->locked);

	m = mark->buf->marks + mark->mark;
	if (m->columnar) {
		if (remove_columnar_mark(mark) == -1)
			return -1;
		if (insert_columnar_text(mark->buf, mark, str, len, 8) == -1)
			return -1;
	} else {
		if (strchange(mark->buf, m->begin, m->len, str, len) == -1)
			return -1;
		sbuf_validate_mark(mark);
	}

	return 0;
}



/* Replace contents of one mark with those of another mark.  The marks may
   overlap.  */
int sbuf_change(Sbufmark *mark, Sbufmark *repl) {
	char *temp, tempbuf[10240];
	long len;
	int ret;

	sbuf_validate_mark(mark);
	sbuf_validate_mark(repl);

	len = sbuf_mark_length(repl);
	if (len < sizeof tempbuf)
		temp = tempbuf;
	else {
		temp = malloc(len+1);
		if (temp == NULL) {
			__publib_error("malloc failed");
			return -1;
		}
	}

	sbuf_strat(temp, repl);
	ret = sbuf_strchange(mark, temp, len);

	if (temp != tempbuf)
		free(temp);

	return ret;
}


/*
 * Function:	sbuf_undo_atomic
 * Purpose:	Undo one entry from the change log.
 */
int sbuf_undo_atomic(Sbuf *buf) {
	struct __sbuf_change *p;
	
	if (buf->log_tail == NULL)
		return -1;
	p = buf->log_tail;
	if (strchange(buf, p->pos, p->newlen, p->oldtext, p->oldlen) == -1)
		return -1;
	return 0;
}


/*
 * Function:	sbuf_dump
 * Purpose:	Dump contents of buffer for debugging purposes.
 */
void sbuf_dump(const char *msg, Sbuf *buf) {
	long i, len;
	int c;

	if (msg != NULL && *msg != '\0')
		printf("%s: ", msg);
	printf("<<");
	len = sbuf_length(buf);
	for (i = 0; i < len; ++i) {
		c = sbuf_charat(buf, i);
		switch (c) {
		case '\n':
			printf("\\n"); break;
		case '\t':
			printf("\\t"); break;
		case '\0':
			printf("\\0"); break;
		case '\\':
			printf("\\\\"); break;
		default:
			printf("%c", c); break;
		}
	}
	printf(">>\n");
}



/*
 * Function:	sbuf_mark_dump
 * Purpose:	Dump contents of mark for debugging purposes.
 */
void sbuf_mark_dump(const char *msg, Sbufmark *mark) {
	char buf[1000];
	long i, len;

	if (msg != NULL && *msg != '\0')
		printf("%s: ", msg);
	printf("%ld-%ld (%ld) col=%d", sbuf_mark_begin(mark), 
		sbuf_mark_end(mark), sbuf_mark_length(mark), 
		sbuf_mark_is_columnar(mark));
	len = sbuf_mark_length(mark);
	if (len >= sizeof(buf))
		printf(" (mark too long for output)\n");
	else {
		sbuf_strat(buf, mark);
		printf(" <<");
		for (i = 0; i < len; ++i) {
			switch (buf[i]) {
			case '\n':
				printf("\\n"); break;
			case '\t':
				printf("\\t"); break;
			case '\0':
				printf("\\0"); break;
			case '\\':
				printf("\\\\"); break;
			default:
				printf("%c", buf[i]); break;
			}
		}
		printf(">>\n");
	}
}



/**********************************************************************
 * Local functions follow.
 */
 

/*
 * Function:	string_dim
 * Purpose:	Return string dimensions as a column.
 */
static void string_dim(const char *s, long len, long *w, long *h) {
	long i, n;
	int prev;
	
	prev = '\n';
	*h = *w = n = 0;
	for (i = 0; i < len; ++i) {
		if (prev == '\n')
			++(*h);
		if (s[i] == '\n') {
			if (n > *w)
				*w = n;
			n = 0;
		} else
			++n;
		prev = s[i];
	}
}



#if UNDO
/*
 * Function:	changelog_entry
 * Purpose:	Create a new changelog entry.
 */
static struct __sbuf_change *changelog_entry(long pos, long newlen,
const char *oldtext, long oldlen, int dirty) {
	struct __sbuf_change *log;

	log = malloc(sizeof(struct __sbuf_change));
	if (log == NULL) {
		__publib_error("out of memory");
		return NULL;
	}

	if (oldlen == 0)
		log->oldtext = NULL;
	else {
		log->oldtext = malloc(oldlen);
		if (log->oldtext == NULL) {
			__publib_error("out of memory");
			free(log);
	 		return NULL;
		}
	}

	log->pos = pos;
	log->newlen = newlen;
	memcpy(log->oldtext, oldtext, oldlen);
	log->oldlen = oldlen;
	log->buffer_was_dirty = dirty;
	log->last_composite = 0;
	log->next = NULL;
	log->prev = NULL;
	
	return log;
}



/*
 * Function:	changelog_add
 * Purpose:	Add new entry to change log. Forget old ones, if log is big.
 */
static void changelog_add(Sbuf *buf, struct __sbuf_change *log) {
	long n, nn;
	struct __sbuf_change *p;

	if (buf->log_tail == NULL) {
		buf->log_head = log;
		buf->log_tail = log;
	} else {
		log->prev = buf->log_tail;
		buf->log_tail->next = log;
		buf->log_tail = log;
	}
	for (n = 0; log != NULL; log = log->prev) {
		nn = n + sizeof(*log) + log->oldlen;
		if (nn > buf->log_max)
			break;
		n = nn;
	}
	if (log != NULL) {
		while (buf->log_head != log) {
			p = buf->log_head;
			buf->log_head = p->next;
			buf->log_head->prev = NULL;
			free(p->oldtext);
			free(p);
		}
	}
}
#endif



/*
 * Function:	replace_mark_part
 * Purpose:	Replace bytes pos..pos+len-1 in the buffer with
 *		the first n bytes from beginning of string, padding the
 *		replacement with spaces so that it fills w bytes, if
 *		necessary.
 * Note:	This can be used to replace the whole contents of a mark,
 *		or to do line-by-line replacements, if the mark is columnar.
 */
static int replace_mark_part(Sbuf *buf, long pos, long len, 
const char *s, long n, long w) {
#if UNDO
	struct __sbuf_change *log;
#endif
	assert(n <= w);
	
	if (sbuf_movegap(buf, pos, w) == -1)
		return -1;

#if UNDO
	log = changelog_entry(pos, w, buf->block + buf->gappos + gaplen(buf),
		len, buf->flags & SBUF_DIRTY_FLAG);
	if (log == NULL)
		return -1;
#endif

	buf->len -= len;

	memcpy(buf->block + buf->gappos, s, n);
	if (n < w)
		memset(buf->block + buf->gappos + n, ' ', w - n);
	buf->len += w;
	buf->gappos += w;
	sbuf_set_dirty(buf, 1);

#if UNDO
	changelog_add(buf, log);
#endif
	
	return 0;
}


/*
 * Function:	adjust_marks
 * Purpose:	The range (begin,len) has been changed to (begin,newlen).
 *		Adjust all marks in buffer accordingly.
 */
static void adjust_marks(Sbuf *buf, long begin, long len, long newlen) {
	struct __sbufmark *m;
	long mend, oldend;
	int i;

	oldend = begin + len;
	for (i = 0; i < buf->markalloc; ++i) {
		m = buf->marks + i;
		if (!m->inuse)
			continue;
		mend = m->begin + m->len;
		if (m->begin < begin && mend > begin) {
			/* m starts before, extends into or after old */
			if (mend <= oldend)
				m->len = newlen + begin - m->begin;
			else
				m->len = m->len - len + newlen;
			m->dirty = 1;
		} else if (m->begin >= begin && m->begin <= oldend) {
			/* m starts within or at beginning of old */
			m->begin = begin;
			if (mend > oldend)
				m->len = newlen + (mend - oldend);
			else
				m->len = newlen;
			m->dirty = 1;
		} else if (m->begin >= oldend) {
			/* m starts at or after end of old */
			m->begin += newlen - len;
		}

		assert(m->begin >= 0);
		assert(m->len >= 0);
		assert(m->begin <= buf->len);
		assert(m->begin + m->len <= buf->len);
	}
}



/*
 * Function:	remove_columnar_mark
 * Purpose:	Remove the selected bit in all lines of a columnar mark.
 */
static int remove_columnar_mark(Sbufmark *mark) {
	long start_pos, end_pos;
	long start_col, end_col;
	long p, pp, q, r;
	const int tabsize = 8;

	start_pos = sbuf_mark_begin(mark);
	end_pos = sbuf_mark_end(mark);
	start_col = sbuf_colno(mark->buf, start_pos, tabsize);
	end_col = sbuf_colno(mark->buf, end_pos, tabsize);
	if (start_col > end_col)
		return -1;

	pp = sbuf_boln(mark->buf, start_pos);
	p = sbuf_boln(mark->buf, end_pos);
	for (; p >= pp; p = sbuf_boln(mark->buf, p-1)) {
		q = sbuf_colpos(mark->buf, p, start_col, tabsize);
		r = sbuf_colpos(mark->buf, p, end_col, tabsize);
		if (replace_mark_part(mark->buf, q, r-q, "", 0, 0) == -1)
			return -1;
/* fixme to use sbuf_adjust_pos_cache */
		sbuf_clear_pos_cache(mark->buf, p);
		adjust_marks(mark->buf, q, r-q, 0);
		sbuf_validate_mark(mark);
	}
	
	return 0;
}



/*
 * Function:	count_chars
 * Purpose:	The number of occurences of a character in a memory area.
 */
static long count_chars(const char *s, int c, long len) {
	long n;
	const char *end;
	
	for (n = 0, end = s+len; s < end; ++s) {
		s = memchr(s, c, end - s);
		if (s == NULL)
			break;
		++n;
	}
	return n;
}



/*
 * Function:	insert_columnar_text
 * Purpose:	Insert a column of text into buffer.
 */
static int insert_columnar_text(Sbuf *buf, Sbufmark *mark, const char *s,
long len, long tab) {
	long i, n, w, h, col, end, pos, begin;

	string_dim(s, len, &w, &h);
	pos = sbuf_mark_begin(mark);
	begin = end = pos;
	col = sbuf_colno(buf, pos, tab);

	for (i = 0; i < len; i += n) {
		for (n = 0; i+n < len && s[i+n] != '\n'; ++n)
			continue;
		if (replace_mark_part(buf, pos, 0, s+i, n, w) == -1)
			return -1;
		adjust_marks(buf, pos, 0, w);
/* fixme to use sbuf_adjust_pos_cache */
		sbuf_clear_pos_cache(buf, pos);
		end = pos + w;
		pos = sbuf_eoln(buf, pos);
		pos = sbuf_colpos(buf, pos, col, tab);
		if (i+n < len && s[i+n] == '\n')
			++n;
	}
	sbuf_remark(mark, begin, end-begin);
	return 0;
}


/*
 * Function:	strchange
 * Purpose:	Replace part of buffer with new text.
 */
static int strchange(Sbuf *buf, long pos, long len, const char *text,
long textlen) {
	long old_lines, new_lines;

	if (sbuf_movegap(buf, pos + len, 0) == -1)
		return -1;

	old_lines = count_chars(buf->block + pos, '\n', len);
	new_lines = count_chars(text, '\n', textlen);

	if (replace_mark_part(buf, pos, len, text, textlen, textlen) == -1)
		return -1;

	sbuf_adjust_pos_cache(buf, pos, len, textlen, new_lines - old_lines);
	adjust_marks(buf, pos, len, textlen);

	return 0;
}
