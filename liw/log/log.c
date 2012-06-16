/*
 * log.c -- log files
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-log:$Id: log.c,v 1.10 1996/04/19 20:38:49 liw Exp $"
 */

#include <assert.h>
#include <time.h>
#include "publib/log.h"
#include "publib/errormsg.h"


#define MAX	256
static struct {
	FILE *f;
	int min;
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
	return nlogs++;
}

void log_set_level(int logid, int min_level) {
	assert(logid >= 0);
	assert(logid < nlogs);
	logs[logid].min = min_level;
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
		char buf[100]; \
		const char *p; \
		int i; \
		if (!log_is_on) return; \
		p = get_progname(); \
		if (*p == '\0') p = "unknown"; \
		tm = NULL; \
		for (i = 0; i < nlogs; ++i) { \
			if (log_level_##level < logs[i].min) continue; \
			if (tm == NULL) { \
				time(&t); \
				tm = gmtime(&t); \
				sprintf(buf, \
				"%04d-%02d-%02d %02d:%02d:%02d UTC %.50s %d ", \
				tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, \
				tm->tm_hour, tm->tm_min, tm->tm_sec, \
				p, log_level_##level); \
			} \
			va_start(args, fmt); \
			(void) fputs(buf, logs[i].f); \
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
	char buf[100];
	const char *p;
	int i;

	if (!log_is_on)
		return;

	p = get_progname();
	if (*p == '\0')
		p = "unknown";
	tm = NULL;

	for (i = 0; i < nlogs; ++i) {
		if (level >= logs[i].min) {
			if (tm == NULL) {
				time(&t);
				tm = gmtime(&t);
				sprintf(buf, 
				"%04d-%02d-%02d %02d:%02d:%02d UTC %.50s %d ",
				tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
				tm->tm_hour, tm->tm_min, tm->tm_sec,
				p, level); 
			}
			va_start(args, fmt);
			(void) fputs(buf, logs[i].f);
			(void) vfprintf(logs[i].f, fmt, args);
			(void) fflush(logs[i].f);
			va_end(args);
		}
	}
}
