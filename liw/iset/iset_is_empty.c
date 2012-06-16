/*
 * iset_is_empty.c -- test whether an iset is empty
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id$"
 */

#include <assert.h>
#include <stddef.h>	/* need NULL */
#include "publib/iset.h"

int iset_is_empty(const Iset *is) {
	assert(is != NULL);
	return is->lowest == NULL;
}
