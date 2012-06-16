/*
 * memoverlap.c -- check whether two memory areas overlap
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-strutil:$Id: memoverlap.c,v 1.3 1994/06/20 20:29:59 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/strutil.h"


/* NOTE: This implementation assumes that arbitrary pointers can be
   compared.  You need to fix it for a system where this doesn't hold.  */
int memoverlap(const void *t, size_t tsize, const void *s, size_t ssize) {
	const char *ct, *cs;

	if (t == NULL || s == NULL)
		return 0;

	ct = t;
	cs = s;
	if (ssize == 0)
		ssize = 1;
	if (tsize == 0)
		tsize = 1;

	return (cs+ssize-1 >= ct && cs <= ct+tsize-1)
	    || (ct+tsize-1 >= cs && ct <= cs+ssize-1);
}


/* As a curiosity, the following should be a strictly standard-conforming
   implementation.  It's rather useless for real work.  */
#if 0
int memoverlap(const void *t, size_t tsize, const void *s, size_t ssize) {
	const char *ct, *cs;
	size_t nt, ns;

	for (ct = t, nt = 0; nt < tsize; ++nt, ++ct)
		for (cs = s, ns = 0; ns < ssize; ++ns, ++cs)
			if (cs == ct)
				return 1;
	return 0;
}
#endif
