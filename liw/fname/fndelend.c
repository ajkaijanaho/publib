/*
 * fndelend.c -- delete last filename in a pathname
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-fname:$Id: fndelend.c,v 1.1.1.1 1993/11/20 17:02:15 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/fname.h"

void fndelend(char *fname) {
	char *p;

	assert(fname != NULL);
	p = strrchr(fname, '/');
	*(p == NULL ? fname : p) = '\0';
}
