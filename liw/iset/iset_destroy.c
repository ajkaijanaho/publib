/*
 * iset_destroy.c -- free resources allocated to an iset
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id$"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/iset.h"

void iset_destroy(Iset *is) {
	assert(is != NULL);
	iset_clear(is);
	free(is);
}
