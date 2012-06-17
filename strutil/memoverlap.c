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
