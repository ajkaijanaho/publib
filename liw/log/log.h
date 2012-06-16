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
