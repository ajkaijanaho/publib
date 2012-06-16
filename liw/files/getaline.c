/*
 * getaline.c -- read arbitrarily long line from file
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-files:$Id: getaline.c,v 1.4 1996/04/28 14:01:19 liw Exp $"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "publib/errormsg.h"
#include "publib/files.h"

char *getaline(FILE *f) {
	char *buf;		/* buffer for line */
	size_t size;		/* size of buffer */
	size_t inc;		/* how much to enlarge buffer */
	size_t len;		/* # of chars stored into buf before '\0' */
	char *p;
	const size_t thres = 128; /* initial buffer size (most lines should
				     fit into this size, so think of this as
				     the "long line threshold").  */
	const size_t mucho = 128; /* if there is at least this much wasted
				     space when the whole buffer has been
				     read, try to reclaim it.  Don't make
				     this too small, else there is too much
				     time wasted trying to reclaim a couple
				     of bytes.  */
	const size_t mininc = 64; /* minimum number of bytes by which
				     to increase the allocated memory */

	len = 0;
	size = thres;
	buf = malloc(size);
	if (buf == NULL) {
		__publib_error("malloc failed");
		return NULL;
	}

	while (fgets(buf+len, size-len, f) != NULL) {
		len += strlen(buf+len);
		if (len > 0 && buf[len-1] == '\n')
			break;		/* the whole line has been read */

		for (inc = size, p = NULL; inc > mininc; inc /= 2)
			if ((p = realloc(buf, size + inc)) != NULL)
				break;

		if (p == NULL) {
			__publib_error("realloc failed");
			free(buf);
			return NULL;	/* couldn't get more memory */
		}

		size += inc;
		buf = p;
	}

	if (len == 0) {
		if (ferror(f))
			__publib_error("I/O error");
		free(buf);
		return NULL;	/* nothing read (eof or error) */
	}

	if (buf[len-1] == '\n')	/* remove newline, if there */
		buf[--len] = '\0';

	if (size - len > mucho) { /* a plenitude of unused memory? */
		p = realloc(buf, len+1);
		if (p != NULL) {
			buf = p;
			size = len+1;
		}
	}

	return buf;
}
