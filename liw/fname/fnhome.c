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
