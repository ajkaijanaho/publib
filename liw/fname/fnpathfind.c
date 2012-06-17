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
 * fnpathfind.c -- find file in a list of directories
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-fname:$Id: fnpathfind.c,v 1.4 1994/07/16 15:28:42 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "publib/fname.h"
#include "publib/errormsg.h"

/* XXX - there is a standard unix function pathfind ... */

int fnpathfind(const char *path, const char *tgt, char *result, size_t max) {
	char *p;		/* auxiliary pointer */
	char *colon;		/* where the next colon is */
	char *fullname;		/* the full name of the current possibility */
	size_t cursize;		/* current block size allocated for fullname */
	size_t fullsize;	/* size of current possibility (<= cursize) */
	size_t dirlen;		/* length of the current component of path */
	size_t tgtlen;		/* length of the tgt */

	assert(path != NULL);
	assert(tgt != NULL);
	assert(result != NULL);
	assert(max > 0);

	tgtlen = strlen(tgt);
	fullname = NULL;
	cursize = 0;

	for (; *path != '\0'; path = colon + (*colon == '\0' ? 0 : 1)) {
		/* find end of current path element (next colon, or the
		   trailing '\0' if last element */
		colon = strchr(path, ':');
		if (colon == NULL)
			colon = strchr(path, '\0');

		/* compute size of full name: 
		      dirname + "/" + tgt + trailing '\0'
		   except that the "/" is only needed if the dirname doesn't
		   end in a slash.  Then make sure fullname is at least big
		   enough.  */
		dirlen = colon - path;
		fullsize = dirlen + (dirlen ? 0 : 1) + tgtlen + 1;
		if (cursize < fullsize) {
			p = realloc(fullname, fullsize);
			if (p == NULL) {
				__publib_error("realloc failed");
				free(fullname);
				return -1;
			}
			fullname = p;
			cursize = fullsize;
		}

		/* construct the full name */
		strncpy(fullname, path, dirlen);
		p = fullname + dirlen;
		if (dirlen > 0 && fullname[dirlen-1] != '/')
			*p++ = '/';
		strcpy(p, tgt);

		/* does it exist?  if so, copy to result and return */
		if (access(fullname, 00) == 00) {
			strncpy(result, fullname, max);
			result[max-1] = '\0';
			free(fullname);
			return fullsize;
		}
	}

	/* we didn't find anything */
	free(fullname);
	return -1;
}
