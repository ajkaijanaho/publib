/*
 * fndelend.c -- delete last filename in a pathname
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-fname:$Id: fndelend.c,v 1.2 1994/06/20 20:49:19 liw Exp $"
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
