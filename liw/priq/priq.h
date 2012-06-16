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
