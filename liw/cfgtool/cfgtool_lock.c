/*
 * File:	cfgtool_lock.c
 * Purpose:	Lock file handling for cfgtool.
 * Author:	Lars Wirzenius
 * Version:	"@(#)publib-cfgtool:$Id: cfgtool_lock.c,v 1.1.1.1 1996/11/05 21:13:54 liw Exp $"
 */

#include <assert.h>
#include "publib/errormsg.h"
#include "publib/lockfile.h"
#include "publib/cfgtool.h"


int cfgtool_lock(Cfgtool *r) {
	assert(r != NULL);

	if (lockfile_create(r->lock_file) == -1) {
		__publib_error("cannot create lock file");
		return -1;
	}

	r->have_lock = 1;
	r->keep_lock = 0;

	return 0;
}


int cfgtool_unlock(Cfgtool *r) {
	assert(r != NULL);
	assert(r->have_lock);

	r->have_lock = 0;
	if (lockfile_remove(r->lock_file) == -1) {
		__publib_error("couldn't remove lock file");
		return -1;
	}
	return 0;
}


void cfgtool_assume_lock(Cfgtool *r) {
	assert(r != NULL);
	r->have_lock = 1;
}


void cfgtool_keep_lock(Cfgtool *r, int x) {
	assert(r != NULL);
	r->keep_lock = x;
}


int cfgtool_keeping_lock(Cfgtool *r) {
	assert(r != NULL);
	return r->keep_lock;
}


int cfgtool_is_locked(Cfgtool *r) {
	assert(r != NULL);
	return r->have_lock;
}
