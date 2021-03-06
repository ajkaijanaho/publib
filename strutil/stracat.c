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
 * stracat.c -- concatenate many strings into an allocated memory block
 *
 * Part of publib.  See man page for more information.
 * "@(#)publib-strutil:$Id: stracat.c,v 1.5 1994/07/16 15:29:17 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "publib/strutil.h"
#include "publib/errormsg.h"


#define N 1024


char *stracat(const char *s, ...) {
	va_list args;
	char *q, *block;
	const char *p;
	size_t size, len;
	size_t lentab[N];
	int n, i;

	assert(s != NULL);

	/*
	 * Compute the amount of memory needed for the target string.
	 *
	 * I could use realloc to make it larger if an initial guess
	 * at its size would be too little, but this way we avoid doing
	 * calling realloc many times, which is a win, because it can
	 * be pretty slow.  However, I haven't actually tested that
	 * this is faster. :-(.
	 *
	 * I use another untested speed hack (but this one should be
	 * obvious -- famous last words): to avoid having to compute the
	 * length of each string twice, I store the lengths in an array,
	 * lentab.  If there are more strings than will fit into lentab,
	 * then the rest will still have their lengths computed twice.
	 * N, the length of lentab, should be made large enough that it
	 * seldom happens, and small enough that there is not significant
	 * memory loss.  On memory-starved machines (such as PCs running
	 * under MS-DOS), one has to take into account also a minuscule stack
	 * size.  On machines with real memory management, a thousand or ten
	 * should be a pretty good value for N.
	 */
	n = 1;
	lentab[0] = strlen(s);
	size = 1 + lentab[0];
	va_start(args, s);
	while ((p = va_arg(args, char *)) != NULL) {
		len = strlen(p);
		size += len;
		if (n < N)
			lentab[n++] = len;
	}
	va_end(args);

	/*
	 * Allocate the block.
	 */
	block = malloc(size);
	if (block == NULL) {
		__publib_error("malloc failed");
		return NULL;
	}

	/*
	 * Concatenate the strings to the allocated block.
	 */
	memcpy(block, s, lentab[0]);
	q = block + lentab[0];
	i = 1;
	va_start(args, s);
	while ((p = va_arg(args, const char *)) != NULL) {
		len = (i < n) ? lentab[i++] : strlen(p);
		memcpy(q, p, len);
		q += len;
	}
	*q = '\0';
	va_end(args);

	return block;
}
