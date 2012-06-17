/* Part of publib.

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
