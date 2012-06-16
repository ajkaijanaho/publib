/*
 * logtest.c -- test the log file routines
 * 
 * Part of publib.  See man page for more information
 * "@(#)publib-log:$Id: test-log.c,v 1.1 1995/09/06 09:23:16 liw Exp $"
 */

#include "publib/log.h"
#include "publib/errormsg.h"

int main(int argc, char **argv) {
	set_progname(argv[0], "logtest");
	log_open("test.log", log_level_error);
	log_chat("1- This shouldn't be printed\n");
	log_error("2- This should be printed\n");
	log_off();
	log_error("3- This shouldn't be printed\n");
	log_on();
	log_error("4- This should be printed\n");
	log_close();
	return 0;
}
