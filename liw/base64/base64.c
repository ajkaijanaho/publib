/*
 * File:	base64.c
 * Purpose:	Implementation of MIME's Base64 encoding and decoding.
 * Author:	Lars Wirzenius
 * Version:	$Id: base64.c,v 1.1.1.1 1996/11/05 21:01:42 liw Exp $
 */

#include <limits.h>
#include "publib/base64.h"

static const char sixtet_to_base64[] = 
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
	int i, j;
	unsigned long w;
	size_t nn;

	w = 0;
	i = 0;
	nn = 0;
	while (n > 0) {
		w |= *from << i*8;
		++from;
		--n;
		++i;
		if (n == 0 || i == 3) {
			for (j = 0; j*6 < i*8; ++j) {
				*to++ = sixtet_to_base64[w & 0x3f];
				++nn;
				w >>= 6;
			}
			if (n == 0)
				for (j = i; j < 3; ++j) {
					*to++ = '=';
					++nn;
				}
			w = 0;
			i = 0;
		}
	}
	return nn;
}


/*
 * Function:	base64_decode
 * Purpose:	Convert base64 to octets.
 * Note:	Output buffer must be at least 3*n/4 chars.
 *		The output buffer does not get a '\0' appended.
 */
size_t base64_decode(char *to, const char *from, size_t len) {
	unsigned long w;
	int i, j;
	static size_t n;
	static char base64_to_sixtet[UCHAR_MAX+1] = {0};
	static int tab_init = 0;

	if (!tab_init) {
		for (i = 0; (j = sixtet_to_base64[i]) != '\0'; ++i)
			base64_to_sixtet[j] = i;
		tab_init = 1;
	}

	w = 0;
	i = 0;
	n = 0;
	while (*from != '\0' && *from != '=' && len-- > 0) {
		if (*from == ' ' || *from == '\n' || *from == '\n') {
			++from;
			continue;
		}
		w |= base64_to_sixtet[* (unsigned char *) from] << i*6;
		++i;
		++from;
		if (*from == '\0' || *from == '=' || i == 4) {
			for (j = 0; j*8 < i*6; ++j) {
				*to++ = w & 0xff;
				++n;
				w >>= 8;
			}
			i = 0;
			w = 0;
		}
	}
	return n;
}
