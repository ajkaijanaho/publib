/*
 * iset_create.c -- create an empty integer set
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-iset:$Id$"
 */

#include <stdlib.h>
#include "publib/iset.h"
#include "publib/errormsg.h"

Iset *iset_create(void) {
	Iset *is;

	is = malloc(sizeof(Iset));
	if (is == NULL) {
		__publib_error("malloc failed");
		return NULL;
	}

	is->lowest = is->highest = NULL;
	return is;
}
