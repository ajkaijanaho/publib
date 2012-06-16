/*
 * File:	base64.h
 * Purpose:	Declarations for MIME's Base64 encoding and decoding.
 * Author:	Lars Wirzenius
 * Version:	$Id: base64.h,v 1.1.1.1 1996/11/05 21:01:42 liw Exp $
 */

#ifndef __publib_base64_h_included
#define __publib_base64_h_included

#include <stddef.h>

size_t base64_length(size_t);
size_t base64_encode(char *, const char *, size_t);
size_t base64_decode(char *, const char *, size_t);

#endif
