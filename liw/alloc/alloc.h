/*
 * publib/alloc.h - memory allocation routines for Publib
 *
 * Lars Wirzenius.  Part of Publib (see publib(3)).
 * "@(#)publib-alloc:$Id: alloc.h,v 1.6 1998/10/25 20:48:37 liw Exp $"
 */

#ifndef __publib_alloc_h
#define __publib_alloc_h

#include <stddef.h>		/* need size_t */

struct dynarr {
	void *data;
	size_t elsize, alloc, used;
};

void dynarr_init(struct dynarr *, size_t);
int dynarr_resize(struct dynarr *, size_t);
int dynarr_copy(struct dynarr *, const struct dynarr *);
void dynarr_free(struct dynarr *);

void *xmalloc(size_t);
void *xrealloc(void *, size_t);
void xfree(void *);

char *xstrdup(const char *);

void *memdup(const void *, size_t);
void *xmemdup(const void *, size_t);

#endif
