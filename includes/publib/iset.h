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
 * iset.h -- sets of integers
 */


#ifndef __publib_iset_h
#define __publib_iset_h

struct __iset_range {
	long start, end;
	struct __iset_range *prev, *next;
};

struct __publib_iset {
	struct __iset_range *lowest, *highest;
};

typedef struct __publib_iset Iset;

Iset *iset_create(void);
void iset_destroy(Iset *__is);
Iset *iset_copy(const Iset *__is);

int iset_is_empty(const Iset *__is);
int iset_add(Iset *__is, long __number);
int iset_add_range(Iset *__is, long __number1, long __number2);
int iset_remove(Iset *__is, long __number);
int iset_remove_range(Iset *__is, long __number1, long __number2);
void iset_clear(Iset *__is);
int iset_contains(const Iset *__is, long __number);
int iset_union(Iset *__is1, const Iset *__is2);
int iset_diff(Iset *__is1, const Iset *__is2);
int iset_isect(Iset *__is1, const Iset *__is2);
void iset_range(const Iset *__is, long *__lowest, long *__highest);
int iset_nth_range(const Iset *__is, long __n, long *__lo, long *__hi);

#endif
