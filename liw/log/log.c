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
 * log.c -- log files
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-log:$Id: log.c,v 1.12 1997/05/09 13:49:16 liw Exp $"
 */

#include <assert.h>
#include <time.h>
#include <unistd.h>
#include "publib/log.h"
#include "publib/errormsg.h"


#define MAX	256
static struct {
	FILE *f;
	int min;
	int use_localtime;
} logs[MAX];
static int nlogs = 0;
static int log_is_on = 1;

int log_open(const char *filename, int min_level) {
	FILE *f;

	assert(filename != NULL);

	assert(nlogs >= 0 && nlogs <= MAX);
	if (nlogs == MAX) {
		__publib_error("too many log files");
		return -1;
	}

	f = fopen(filename, "a");
	if (f == NULL) {
		__publib_error("fopen failed");
		return -1;
	}

	logs[nlogs].f = f;
	logs[nlogs].min = min_level;
	logs[nlogs].use_localtime = 0;
	return nlogs++;
}

int log_add(FILE *f, int min_level) {
	assert(f != NULL);

	assert(nlogs >= 0 && nlogs <= MAX);
	if (nlogs == MAX) {
		__publib_error("too many log files");
		return -1;
	}

	logs[nlogs].f = f;
	logs[nlogs].min = min_level;
	logs[nlogs].use_localtime = 0;
	return nlogs++;
}

void log_set_level(int logid, int min_level) {
	assert(logid >= 0);
	assert(logid < nlogs);
	logs[logid].min = min_level;
}


void log_set_localtime(int logid, int use_localtime) {
	assert(logid >= 0);
	assert(logid < nlogs);
	logs[logid].use_localtime = use_localtime;
}


int log_close(void) {
	int i, ret;

	assert(nlogs >= 0 && nlogs <= MAX);
	ret = 0;
	for (i = 0; i < nlogs; ++i) {
		if (fclose(logs[i].f) != 0) {
			__publib_error("fclose failed for logfile");
			ret = -1;
		}
	}
	return ret;
}


void log_off(void) {
	log_is_on = 0;
}


void log_on(void) {
	log_is_on = 1;
}


/* All the functions log_chat through log_fatal are exactly the same,
 * except for the level argument to log_vprintf.  Therefore, we use
 * a macro to emphasize the similarity and to avoid unnecessary
 * differences.
 */
#define do_log_at_level(level) \
	void log_##level(const char *fmt, ...) { \
		va_list args; \
		struct tm *tm; \
		time_t t; \
		char buf1[100]; \
		char buf2[200]; \
		const char *p; \
		int i; \
		long pid; \
		if (!log_is_on) return; \
		p = get_progname(); \
		if (*p == '\0') p = "unknown"; \
		tm = NULL; \
		pid = (long) getpid(); \
		for (i = 0; i < nlogs; ++i) { \
			if (log_level_##level < logs[i].min) continue; \
			if (tm == NULL) { \
				time(&t); \
				if (logs[i].use_localtime) \
					tm = localtime(&t); \
				else \
					tm = gmtime(&t); \
				strftime(buf1, sizeof(buf1), \
				"%Y-%m-%d %H-%M-%S %Z %%.50s %%ld %%d ", tm); \
				sprintf(buf2, buf1, p, pid, log_level_##level);\
			} \
			va_start(args, fmt); \
			(void) fputs(buf2, logs[i].f); \
			(void) vfprintf(logs[i].f, fmt, args); \
			(void) fflush(logs[i].f); \
			va_end(args); \
		} \
	}
do_log_at_level(chat)
do_log_at_level(note)
do_log_at_level(warn)
do_log_at_level(error)
do_log_at_level(fatal)


void log_printf(int level, const char *fmt, ...) {
	va_list args;
	struct tm *tm;
	time_t t;
	char buf1[100];
	char buf2[100];
	const char *p;
	int i;
	long pid;

	if (!log_is_on)
		return;

	p = get_progname();
	if (*p == '\0')
		p = "unknown";
	tm = NULL;
	pid = (long) getpid();

	for (i = 0; i < nlogs; ++i) {
		if (level >= logs[i].min) {
			if (tm == NULL) {
				time(&t);
				if (logs[i].use_localtime)
					tm = localtime(&t);
				else
					tm = gmtime(&t);
				strftime(buf1, sizeof(buf1),
				"%Y-%m-%d %H-%M-%S %Z %%.50s %%ld %%d ", tm);
				sprintf(buf2, buf1, p, pid, level);
			}
			va_start(args, fmt);
			(void) fputs(buf2, logs[i].f);
			(void) vfprintf(logs[i].f, fmt, args);
			(void) fflush(logs[i].f);
			va_end(args);
		}
	}
}
