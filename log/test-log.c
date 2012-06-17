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
 * logtest.c -- test the log file routines
 * 
 * Part of publib.  See man page for more information
 * "@(#)publib-log:$Id: test-log.c,v 1.3 1997/05/09 13:49:16 liw Exp $"
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
