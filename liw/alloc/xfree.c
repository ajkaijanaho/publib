/*
 * xfree.c -- frontend for free (mostly for symmetricity with x*alloc)
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-alloc:$Id: xfree.c,v 1.1.1.1 1993/11/20 17:01:28 liw Exp $"
 */

#include <stdlib.h>
#include "publib/alloc.h"

void xfree(void *p) {
	if (p != NULL)
		free(p);
}
