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
 * File:	base64.c
 * Purpose:	Implementation of MIME's Base64 encoding and decoding.
 * Author:	Lars Wirzenius
 * Version:	$Id: base64.c,v 1.2 2003/11/15 18:24:10 liw Exp $
 */

#include <limits.h>
#include "publib/base64.h"

static const unsigned char sixtet_to_base64[] = 
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


/*
 * Function:	base64_length
 * Purpose:	Compute minimum length of encoded output.
 */
size_t base64_length(size_t n) {
	return 4*(n+3)/3 + 1;
}


/*
 * Function:	base64_encode
 * Purpose:	Convert octets to base64
 * Note:	Output buffer must be at least base64_length(n) chars.
 */
size_t base64_encode(char *to, const char *from, size_t n) {
    	unsigned w;
	char *to_start;
	unsigned char *fromp;
	
    	to_start = to;

    	fromp = (unsigned char *) from;
    	for (; n >= 3; n -= 3, fromp += 3) {
	    w = (fromp[0] << 16) | (fromp[1] << 8) | fromp[2];
	    *to++ = sixtet_to_base64[(w >> 18) & 0x3f];
	    *to++ = sixtet_to_base64[(w >> 12) & 0x3f];
	    *to++ = sixtet_to_base64[(w >> 6) & 0x3f];
	    *to++ = sixtet_to_base64[w & 0x3f];
	}

    	switch (n) {
	case 0:
	    /* Nothing to do */
	    break;
    	
	case 1:
	    w = fromp[0];
	    *to++ = sixtet_to_base64[(w >> 2) & 0x3f];
	    *to++ = sixtet_to_base64[(w << 4) & 0x3f];
	    *to++ = '=';
	    *to++ = '=';
	    break;

	case 2:
	    w = (fromp[0] << 8) | fromp[1];
	    *to++ = sixtet_to_base64[(w >> 10) & 0x3f];
	    *to++ = sixtet_to_base64[(w >> 4) & 0x3f];
	    *to++ = sixtet_to_base64[(w << 2) & 0x3f];
	    *to++ = '=';
	    break;
	}
	
	return to - to_start;
}


/*
 * Function:	base64_decode
 * Purpose:	Convert base64 to octets.
 * Note:	Output buffer must be at least 3*n/4 chars.
 *		The output buffer does not get a '\0' appended.
 */
size_t base64_decode(char *to, const char *from, size_t len) {
    	static int base64_to_sixtet[UCHAR_MAX + 1];
	static int tab_init = 0;
	int i;
	unsigned bitbuf;
	int nbits;
	unsigned char *fromp;
	char *to_start;

    	if (!tab_init) {
	    tab_init = 1;
	    for (i = 0; i <= UCHAR_MAX + 1; ++i)
	    	base64_to_sixtet[i] = -1;
	    for (i = 0; sixtet_to_base64[i] != '\0'; ++i)
	    	base64_to_sixtet[sixtet_to_base64[i]] = i;
	}

    	to_start = to;

    	bitbuf = 0;
	nbits = 0;
	fromp = (unsigned char *) from;
    	for (i = 0; i < len && fromp[i] != '='; ++i) {
	    if (base64_to_sixtet[fromp[i]] != -1) {
		bitbuf = (bitbuf << 6) | base64_to_sixtet[fromp[i]];
		nbits += 6;
		if (nbits >= 8) {
		    *to++ = (bitbuf >> (nbits - 8)) & 0xff;
		    bitbuf >>= 8;
		    nbits -= 8;
		}
	    }
	}
	
	return to - to_start;
}
