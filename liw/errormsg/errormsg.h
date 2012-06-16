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
