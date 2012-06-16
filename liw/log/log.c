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
