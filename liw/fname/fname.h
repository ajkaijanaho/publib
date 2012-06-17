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
 * fname.h -- filename manipulation
 *
 * Part of publib.
 * "@(#)publib-fname:$Id: fname.h,v 1.1.1.1 1993/11/20 17:02:15 liw Exp $"
 */

#ifndef __publib_fname_h
#define __publib_fname_h

#include <stddef.h>	/* need size_t */

char *fnbase(const char *);
void fndelbeg(char *);
void fndelend(char *);
int fndelsuf(char *, const char *);
void fndir(char *, const char *);
int fnhome(char *, const char *);
void fnjoin(char *, const char *, const char *);
char *fnlastsuf(const char *);
int fnpathfind(const char *, const char *, char *, size_t);
size_t fnqualify(char *, const char *, size_t);
size_t fnsetsuf(char *, const char *, size_t);

#endif
