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
 * File:	priq.h
 * Purpose:	Declarations for priority queues.
 * Author:	Lars Wirzenius
 * Version:	"@(#)publib:$Id: priq.h,v 1.2 1997/01/01 19:38:46 liw Exp $"
 */

#ifndef __publib_priq_h_included
#define __publib_priq_h_included

typedef struct priq Priq;

Priq *priq_create(int (*)(const void *, const void *));
void priq_destroy(Priq *);
void priq_set_compare(Priq *, int (*)(const void *, const void *));

int priq_insert(Priq *, const void *, size_t);
void *priq_remove(Priq *);
int priq_is_empty(Priq *);

void priq_dump(Priq *);

#ifdef __publib__

#include <publib/alloc.h>
struct priq {
	struct dynarr da;
	int (*compare)(const void *, const void *);
};

#endif

#endif
