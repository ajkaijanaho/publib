/*
 * fnqualify.c -- qualify a filename
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-fname:$Id: fnqualify.c,v 1.5 1996/08/12 01:03:57 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include "publib/fname.h"
#include "publib/errormsg.h"

#ifndef USERNAME_MAX
#define USERNAME_MAX 9
#endif

size_t fnqualify(char *result, const char *path, size_t max) {
	size_t len, size;
	struct passwd *pwd;
	const char *p;
	char username[USERNAME_MAX];

	assert(result != NULL);
	assert(path != NULL);
	assert(max > 0);

	/* Is it qualified already? */
	if (path[0] == '/') {
		strncpy(result, path, max);
		result[max-1] = '\0';
		return strlen(path) + 1;
	}

	/* Do we just need to prepend the current directory? */
	if (path[0] != '~') {
		if (getcwd(result, max) == NULL) {
			__publib_error("getcwd failed");
			return -1;
		}
		len = strlen(result);
		size = len + 1 + strlen(path) + 1; /* +2 for '/' and '\0' */
		if (size > max)
			return size;
		sprintf(result + len, "/%s", path);
		return size;
	}
		
	/* We need to do tilde substitution, get the password entry (which
	   includes the name of the home directory) */
	if (path[1] == '\0' || path[1] == '/') {
		pwd = getpwuid(getuid());
		if (path[1] == '\0')
			p = path + 1;
		else
			p = path + 2;
	} else {
		p = strchr(path, '/');
		if (p == NULL)
			p = strchr(path, '\0');
		size = (size_t) (p-path);
		if (size > sizeof(username))
			return -1;
		memcpy(username, path+1, size);
		username[size-1] = '\0';

		pwd = getpwnam(username);
		if (*p == '/')
			++p;
	}
	if (pwd == NULL)
		return -1;

	/* Now we have all the necessary information, build the result */
	size = strlen(pwd->pw_dir) + 1 + strlen(p) + 1;
	if (size < max)
		sprintf(result, "%s/%s", pwd->pw_dir, p);
	return size;
}
