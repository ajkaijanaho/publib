/*
 * cmp.c -- comparison function for qsort, bsearch, and others
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-cmp:$Id: cmp.c,v 1.1.1.1 1993/11/20 17:01:45 liw Exp $"
 */

#include <assert.h>
#include <string.h>

#include "publib/cmp.h"

static size_t offset = 0;
static int (*member_cmp)(const void *, const void *) = NULL;

void cmp_set_offset(size_t off, int (*cmp)(const void *, const void *)) {
	assert(cmp != cmp_struct);
	offset = off;
	member_cmp = cmp;
}

int cmp_struct(const void *e1, const void *e2) {
	const char *p1 = e1;
	const char *p2 = e2;
	assert(member_cmp != NULL);
	return member_cmp(p1 + offset, p2 + offset);
}

#define CAT(a,b)	a##b

#define SIMPLE_CMP(name, type)					\
	int CAT(cmp_,name) (const void *e1, const void *e2) {	\
		type p1 = *(const type *)e1;			\
		type p2 = *(const type *)e2;			\
		if (p1 < p2)					\
			return -1;				\
		if (p1 == p2)					\
			return 0;				\
		return 1;					\
	}


SIMPLE_CMP(char,	char)
SIMPLE_CMP(short,	short)
SIMPLE_CMP(int,		int)
SIMPLE_CMP(long,	long)

SIMPLE_CMP(schar,	signed char)

SIMPLE_CMP(uchar,	unsigned char)
SIMPLE_CMP(ushort,	unsigned short)
SIMPLE_CMP(uint,	unsigned int)
SIMPLE_CMP(ulong,	unsigned long)

SIMPLE_CMP(float,	float)
SIMPLE_CMP(double,	double)
SIMPLE_CMP(ldouble,	long double)


int cmp_charptr(const void *e1, const void *e2) {
	return strcmp(*(const char **)e1, *(const char **)e2);
}


int cmp_chararr(const void *e1, const void *e2) {
	return strcmp((const char *)e1, (const char *)e2);
}
