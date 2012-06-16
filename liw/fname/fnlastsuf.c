/*
 * fnlastsuf.c -- find last suffix in basename of filename
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-fname:$Id: fnlastsuf.c,v 1.1.1.1 1993/11/20 17:02:15 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/fname.h"

char *fnlastsuf(const char *fname) {
	assert(fname != NULL);
	return strchr(fnbase(fname), '.');
}
