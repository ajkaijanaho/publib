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
 * fnjoin.c -- join two filenames together
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-fname:$Id: fnjoin.c,v 1.2 1994/06/20 20:49:23 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "publib/fname.h"

void fnjoin(char *res, const char *f1, const char *f2) {
	const char *p;

	assert(res != NULL);
	assert(f1 != NULL);
	assert(*f1 != '\0');
	assert(f2 != NULL);
	assert(*f2 != '\0');

	p = strchr(f1, '\0');
	if ((p > f1 && p[-1] == '/') || *f2 == '/')
		p = "";
	else
		p = "/";
	sprintf(res, "%s%s%s", f1, p, f2);
}
