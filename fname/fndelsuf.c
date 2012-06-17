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
 * fndelsuf.c -- delete last suffix from basename of filename
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-fname:$Id: fndelsuf.c,v 1.2 1994/06/20 20:49:20 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/fname.h"

int fndelsuf(char *fname, const char *suffix) {
	char *dot;

	assert(fname != NULL);
	assert(suffix != NULL);
	assert(*suffix != '\0');
	dot = fnlastsuf(fname);
	if (dot == NULL)
		return -1;
	if (suffix == NULL || strcmp(dot, suffix) == 0) {
		*dot = '\0';
		return 0;
	}
	return -1;
}