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
