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
