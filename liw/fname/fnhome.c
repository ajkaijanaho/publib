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
 * fnhome.c -- return home directory of user
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-fname:$Id: fnhome.c,v 1.4 1994/07/16 15:28:39 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include "publib/fname.h"
#include "publib/errormsg.h"

int fnhome(char *homedir, const char *username) {
	struct passwd *pwd;

	assert(homedir != NULL);
	assert(username != NULL);
	assert(*username != '\0');
	if (username == NULL)
		pwd = getpwuid(getuid());
	else
		pwd = getpwnam(username);
	if (pwd == NULL) {
		__publib_error("can't find home directory");
		return -1;
	}

	strcpy(homedir, pwd->pw_dir);
	return 0;
}
