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
 * errormsg.h -- declarations for error message printing
 *
 * Part of publib
 * "@(#)publib-errormsg:$Id: errormsg.h,v 1.1.1.1 1995/08/06 21:57:17 liw Exp $"
 */

#ifndef __publib_errormsg_h
#define __publib_errormsg_h

/*
 * The following are meant for user programs.
 */
void set_progname(const char *__argv0, const char *__default);
const char *get_progname(void);
void errormsg(int __exitp, int __errno, const char *_fmt, ...);


/*
 * The rest is meant for internal use by the publib library.
 */

enum __liberror {
	__exit_on_error = 0x01,
	__abort_on_error = 0x02,
	__complain_on_error = 0x04
};
extern enum __liberror __liberror;

void __set_liberror(enum __liberror);


#define __publib_error(msg) \
	((__liberror & __complain_on_error) \
	 && (errormsg(__liberror & (__abort_on_error | __exit_on_error), -1, \
		      "error inside publib library function: %s:%ld: %s",  \
		      __FILE__, (int)__LINE__, (msg)), 0))

#endif
