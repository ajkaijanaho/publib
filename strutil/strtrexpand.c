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
 * strtrexpand.c -- expand tr-like notation in string
 *
 * Part of publib.  See man page for more information.
 * "@(#)publib-strutil:$Id: strtrexpand.c,v 1.1 1994/06/20 20:30:43 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "publib/strutil.h"


/* a-b: everything from charcode a to charcode b
   \a: the character a
   \o \oo \ooo : character with octal code o
*/
#define odigit(c)	(c >= '0' && c <= '7')
void strtrexpand(char *tgt, const char *src) {
	int c;
	while (src[0] != '\0') {
		if (src[0] == '\\' && odigit(src[1])) {
			++src;
			c = *src++ - '0';
			if (odigit(*src))
				c = c*8 + *src++ - '0';
			if (odigit(*src))
				c = c*8 + *src++ - '0';
			*tgt++ = c;
		} else if (src[0] == '\\') {
			*tgt++ = src[1];
			src += 2;
		} else if (src[1] == '-' && src[2] != '\0') {
			for (c = src[0]; c <= src[2]; ++c)
				*tgt++ = c;
			src += 3;
		} else {
			*tgt++ = *src++;
		}
	}
	*tgt = '\0';
}
