/*
 * xfree.c -- frontend for free (mostly for symmetricity with x*alloc)
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-alloc:$Id$"
 */

#include <stdlib.h>
#include "publib/alloc.h"

void xfree(void *p) {
	if (p != NULL)
		free(p);
}
