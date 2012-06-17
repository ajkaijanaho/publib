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
 * cmp.h -- comparison functions for qsort, bsearch, et al.
 *
 * Part of publib.
 * "@(#)publib-cmp:$Id: cmp.h,v 1.1.1.1 1993/11/20 17:01:45 liw Exp $"
 */

#ifndef __publib_cmp_h
#define __publib_cmp_h

#include <stddef.h>	/* size_t */

void cmp_set_offset(size_t __offset, int (*)(const void *, const void *));
int cmp_struct(const void *, const void *);

int cmp_char(const void *, const void *);
int cmp_short(const void *, const void *);
int cmp_int(const void *, const void *);
int cmp_long(const void *, const void *);

int cmp_schar(const void *, const void *);

int cmp_uchar(const void *, const void *);
int cmp_ushort(const void *, const void *);
int cmp_uint(const void *, const void *);
int cmp_ulong(const void *, const void *);

int cmp_float(const void *, const void *);
int cmp_double(const void *, const void *);
int cmp_ldouble(const void *, const void *);

int cmp_charptr(const void *, const void *);
int cmp_chararr(const void *, const void *);

#endif
