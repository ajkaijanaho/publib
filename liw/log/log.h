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
 * log.h -- log files
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-log:$Id: log.h,v 1.4 1997/05/09 13:43:49 liw Exp $"
 */

#ifndef __publib_log_h_included
#define __publib_log_h_included

#include <stdio.h>	/* need FILE */
#include <stdarg.h>	/* need va_list */

enum log_level {
	log_level_chat,
	log_level_note,
	log_level_warn,
	log_level_error,
	log_level_fatal
};

int log_open(const char *, int);
int log_add(FILE *, int);
void log_set_level(int, int);
void log_set_localtime(int, int);
int log_close(void);
void log_off(void);
void log_on(void);
void log_chat(const char *, ...);
void log_note(const char *, ...);
void log_warn(const char *, ...);
void log_error(const char *, ...);
void log_fatal(const char *, ...);
void log_printf(int, const char *, ...);

#endif
