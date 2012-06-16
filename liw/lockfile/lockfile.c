/*
 * File:	lockfile.c
 * Purpose:	Create a lock file.
 * Author:	Lars Wirzenius
 * Version:	$Id: lockfile.c,v 1.1.1.1 1996/11/05 21:06:56 liw Exp $
 */


#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
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
