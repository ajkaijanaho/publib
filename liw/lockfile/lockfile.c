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
 * File:	lockfile.c
 * Purpose:	Create a lock file.
 * Author:	Lars Wirzenius
 * Version:	$Id: lockfile.c,v 1.2 1997/04/26 23:52:21 liw Exp $
 */


#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <publib.h>

#include "lockfile.h"



/*
 * Function:	lockfile_create
 * Purpose:	Create a lock file.
 */
int lockfile_create(const char *lockname) {
	char tempname[FILENAME_MAX + 256];	/* enough for template */
	char dir[FILENAME_MAX];
	struct stat st;
	int e, fd;

	if (strlen(lockname) > FILENAME_MAX) {
		errno = ENAMETOOLONG;
		return -1;
	}
	
	fndir(dir, lockname);
	fnjoin(tempname, dir, ".temp-lock");
	fd = open(tempname, O_CREAT | O_EXCL, 0600);
	if (fd == -1)
		return -1;
	(void) close(fd);

	if (link(tempname, lockname) == -1
	    || stat(lockname, &st) == -1
	    || st.st_nlink != 2) {
		e = errno;
		(void) unlink(tempname);
		errno = e;
		return -1;
	}

	(void) unlink(tempname);
	return 0;
}


/*
 * Function:	lockfile_remove
 * Purpose:	Remove a lock file.
 */
int lockfile_remove(const char *lockname) {
	return unlink(lockname);
}
