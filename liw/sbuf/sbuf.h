/*
 * sbuf.h -- simple text editor buffer routines
 *
 * Part of Publib.  See manpage for more information.
 * "@(#)publib-sbuf:$Id: sbuf.h,v 1.14 1996/07/16 12:31:04 liw Exp $"
 */


#ifndef __publib_sbuf_included
#define __publib_sbuf_included


/*
 * Includes for types defined elsewhere.
 * These are all standard headers or Publib headers.
 */

#include <stdio.h>		/* need size_t and FILE */
#include <publib/alloc.h>	/* need struct dynarr */


struct __sbufmark {
	unsigned inuse:1;	/* is this mark in use? */
	unsigned dirty:1;	/* has text in mark been modified? */
	unsigned columnar:1;	/* is the mark columnar? */
	long begin;		/* where the buffer begins ... */
	long len;		/* ...and how long it is */
};



/*
 * sbuf_lineno caches the values it computes.  The cache
 * contains entries of the following type.
 */
struct __sbuf_pos_cache {
	long pos;
	long lineno;
};
#define SBUF_POS_CACHE_MAX (50000)
#define SBUF_POS_CACHE_MIN (16)

#if SBUF_POS_CACHE_MAX < SBUF_POS_CACHE_MIN
#error Do not make SBUF_POS_CACHE_MAX smaller than SBUF_POS_CACHE_MIN.
#endif

#if SBUF_POS_CACHE_MIN < 16
#error Do not make SBUF_POS_CACHE_MIN too small.
#endif


struct __sbuf {
	char *block;
	unsigned locked:1;
	unsigned dirty:1;
	size_t gappos;
	size_t alloc;
	size_t len;

	struct __sbufmark *marks;
	size_t markalloc;

#if 0
	struct __sbuf_pos_cache poscache[SBUF_POS_CACHE_MAX];
	int poscount;
#else
	struct dynarr pc;
	long poshits, posmisses;
	double posmissavg;
#endif
};
typedef struct __sbuf Sbuf;

/*
 * This is how the client sees a struct __bufmark.  We can't use a
 * pointer directly, since the struct __bufmarks may move because of
 * realloc, and we can't assume that the client will be able to update
 * his pointers.
 */
typedef struct {
	Sbuf *buf;
	size_t mark;
} Sbufmark;


/*
 * The validation functions shouldn't generate any code when NDEBUG is
 * defined.  This is the same behaviour as assert has.  Note that the
 * prototypes and definitions of these functions are protected against
 * the macros by virtue of having the function names within parentheses.
 */
#ifdef NDEBUG
#define sbuf_validate(buf)	((void)0)
#define sbuf_validate_mark(m)	((void)0)
#endif


/*
 * Let's use the same NDEBUG flag to turn on a macro version of sbuf_charat.
 * _NO_ error checking for this baby.  Use the non-macro version for that.
 */
#ifdef NDEBUG
#include <stdio.h> /* need EOF */
#define sbuf_charat(buf, pos) \
	((pos) == (buf)->len \
		?  EOF \
		: (int) (unsigned char) ((pos) < (buf)->gappos \
			? (buf)->block[(pos)] \
			: (buf)->block[(pos) + (buf)->alloc - (buf)->len]))
#endif


/*
 * Er..., did someone say NDEBUG for macros?  Some of the status/info routines
 * are used pretty heavily as well...
 */
#ifdef NDEBUG
#define sbuf_length(b)		 ((b)->len)
#define sbuf_mark_begin(m)	 ((m)->buf->marks[(m)->mark].begin)
#define sbuf_mark_length(m)	 ((m)->buf->marks[(m)->mark].len)
#define sbuf_mark_end(m)	 (sbuf_mark_begin(m) + sbuf_mark_length(m))
#define sbuf_mark_is_dirty(m)	 ((m)->buf->marks[(m)->mark].dirty)
#define sbuf_mark_set_dirty(m,d) ((m)->buf->marks[(m)->mark].dirty = (d))
#endif



/*
 * Prototypes for functions provided by sbuf.
 */

Sbuf *sbuf_create(void);
void sbuf_destroy(Sbuf *);
void (sbuf_validate)(Sbuf *);

int sbuf_is_dirty(Sbuf *);
void sbuf_set_dirty(Sbuf *, int);

Sbufmark *sbuf_mark(Sbuf *, long, long);
void sbuf_remark(Sbufmark *, long, long);
void sbuf_unmark(Sbufmark *);
long (sbuf_mark_begin)(Sbufmark *);
long (sbuf_mark_end)(Sbufmark *);
long (sbuf_mark_length)(Sbufmark *);
int (sbuf_mark_is_dirty)(Sbufmark *);
void (sbuf_mark_set_dirty)(Sbufmark *, int);
void (sbuf_validate_mark)(Sbufmark *);
int sbuf_mark_is_columnar(Sbufmark *);
void sbuf_mark_set_columnar(Sbufmark *, int);

int sbuf_strchange(Sbufmark *, const char *, size_t);
int sbuf_change(Sbufmark *, Sbufmark *);
int (sbuf_charat)(Sbuf *, long);
void sbuf_strat(char *, Sbufmark *);

int sbuf_insert_file(Sbufmark *, FILE *);
int sbuf_write_to_file(Sbufmark *, FILE *);

long (sbuf_length)(Sbuf *);

char *sbuf_lock(Sbuf *);
void sbuf_unlock(Sbuf *);

long sbuf_boln(Sbuf *, long);
long sbuf_eoln(Sbuf *, long);
long sbuf_bow(Sbuf *, long);
long sbuf_eow(Sbuf *, long);
long sbuf_lineno(Sbuf *, long);
long sbuf_linepos(Sbuf *, long);
long sbuf_colno(Sbuf *, long, long);
long sbuf_colpos(Sbuf *, long, long, long);

long sbuf_find_pair(Sbuf *, long);

void sbuf_clear_pos_cache(Sbuf *, long);
void sbuf_adjust_pos_cache(Sbuf *, long, long, long, long);
void sbuf_cache_stats(Sbuf *, FILE *);

#define SBUF_BACKWARD		0x01
#define SBUF_IGNORE_CASE	0x02

int sbuf_search(Sbufmark *, Sbufmark *, const char *, size_t, unsigned long);

void sbufgap_movegap(Sbuf *, size_t);

#endif
