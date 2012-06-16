/*
 * logtest.c -- test the log file routines
 * 
 * Part of publib.  See man page for more information
 * "@(#)publib-log:$Id: test-log.c,v 1.2 1997/05/09 13:43:49 liw Exp $"
 */

#include "publib/log.h"
#include "publib/errormsg.h"

int main(int argc, char **argv) {
	int i;
	
	set_progname(argv[0], "logtest");
	i = log_open("test.log", log_level_error);
	log_set_localtime(i, 1);
	log_chat("1- This shouldn't be printed\n");
	log_error("2- This should be printed\n");
	log_off();
	log_error("3- This shouldn't be printed\n");
	log_on();
	log_error("4- This should be printed\n");
	log_printf(log_level_error, "5- This should be printed\n");
	log_close();
	return 0;
}
