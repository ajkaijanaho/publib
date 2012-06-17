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
 * tbuf.h -- editor buffer implemented with a tree
 *
 * Part of Publib, See manual pages publib(3) and tbuf(3) for more information.
 * "@(#)publib-tbuf:$Id: tbuf.h,v 1.4 1998/12/23 18:03:21 liw Exp $"
 */

#ifndef __publib_tbuf_h_included
#define __publib_tbuf_h_included

#include <stddef.h>

typedef struct tbuf Tbuf;

Tbuf *tbuf_create(const char *, size_t);
void tbuf_destroy(Tbuf *);
Tbuf *tbuf_copy(Tbuf *, size_t, size_t);
Tbuf *tbuf_cat(Tbuf *, Tbuf *);
size_t tbuf_length(Tbuf *);
void tbuf_chars(char *, Tbuf *, size_t, size_t);

struct tbuf_stat {
	Tbuf *tbuf;
	size_t tbuf_length;
	size_t tbuf_height;
	size_t tbuf_nodes;
	size_t tbuf_chunks;
	size_t tbuf_memory_total;
	size_t tbuf_nodes_with_few_kids;
};
void tbuf_get_stats(struct tbuf_stat *, Tbuf *);

#endif
