/*
 * iset_destroy.c -- free resources allocated to an iset
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id: iset_destroy.c,v 1.1.1.1 1993/11/20 17:02:31 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/iset.h"

void iset_destroy(Iset *is) {
	assert(is != NULL);
	iset_clear(is);
	free(is);
}
