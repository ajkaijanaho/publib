/*
 * dynarr.c -- dynamic arrays
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-alloc:$Id: dynarr.c,v 1.5 1995/11/26 10:13:06 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "publib/alloc.h"
#include "publib/errormsg.h"


void dynarr_init(struct dynarr *da, size_t elsize) {
	assert(da != NULL);

	da->data = NULL;
	da->alloc = da->used = 0;
	da->elsize = elsize;
}


int dynarr_resize(struct dynarr *da, size_t newsize) {
	void *p;

	assert(da != NULL);
	assert(newsize > 0);

	if ((p = realloc(da->data, newsize * da->elsize)) == NULL)
		return -1;
	da->data = p;
	da->alloc = newsize;
	if (da->used > newsize)
		da->used = newsize;
	return 0;
}


int dynarr_copy(struct dynarr *copy, const struct dynarr *orig) {
	dynarr_init(copy, orig->elsize);
	if (orig->used > 0) {
		if (dynarr_resize(copy, orig->used) == -1)
			return -1;
		memcpy(copy->data, orig->data, orig->elsize * orig->used);
	}
	copy->used = orig->used;
	return 0;
}


void dynarr_free(struct dynarr *da) {
	assert(da != NULL);

	free(da->data);
	dynarr_init(da, da->elsize);
}
