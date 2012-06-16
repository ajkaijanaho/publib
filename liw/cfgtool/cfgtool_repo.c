/*
 * File:	cfgtool_repo.c
 * Purpose:	Implementation of library routines handling Debian's cfgtool.
 * Author:	Lars Wirzenius
 * Version:	"@(#)publib-cfgtool:$Id: cfgtool_repo.c,v 1.1.1.1 1996/11/05 21:13:54 liw Exp $"
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "publib/alloc.h"
#include "publib/files.h"
#include "publib/errormsg.h"
#include "publib/cfgtool.h"

Cfgtool *cfgtool_init_repository(void) {
	Cfgtool *r;

	r = malloc(sizeof(Cfgtool));
	if (r == NULL) {
		__publib_error("out of memory?");
		return NULL;
	}
	
	dynarr_init(&r->da, 1);
	r->lock_file = NULL;
	r->repository = NULL;
	r->value_file = NULL;
	r->loaded = 0;
	r->dirty = 0;
	r->have_lock = 0;
	r->keep_lock = 0;
	
	return r;
}



int cfgtool_name_repository(Cfgtool *r, const char *name) {
	char *repository, *lock_file, *value_file;

	repository = strdup(name);
	if (repository == NULL) {
		__publib_error("out of memory?");
		return -1;
	}

	lock_file = malloc(strlen(repository) + strlen("/lock") + 1);
	value_file = malloc(strlen(repository) + strlen("/values") + 1);
	if (lock_file == NULL || value_file == NULL) {
		__publib_error("out of memory?");
		free(lock_file);
		free(value_file);
		free(repository);
		return -1;
	}
	
	free(r->lock_file);
	free(r->repository);
	free(r->value_file);
	
	r->lock_file = lock_file;
	r->repository = repository;
	r->value_file = value_file;
	
	sprintf(r->lock_file, "%s/lock", r->repository);
	sprintf(r->value_file, "%s/values", r->repository);
	
	return 0;
}


int cfgtool_repository_is_dirty(Cfgtool *r) {
	return r->dirty;
}


int cfgtool_repository_is_loaded(Cfgtool *r) {
	return r->loaded;
}


int cfgtool_load_repository(Cfgtool *r) {
	dynarr_init(&r->da, 1);
	if (file_read(r->value_file, &r->da.data, &r->da.used) == -1)
		return -1;
	r->da.alloc = r->da.used;
	r->dirty = 0;
	r->loaded = 1;
	return 0;
}


int cfgtool_create_disk_repository(Cfgtool *r) {
	int fd;

	if (mkdir(r->repository, 0755) == -1)
		return -1;
	
	fd = open(r->value_file, O_CREAT, 0666);
	if (fd == -1)
		return -1;
	(void) close(fd);
	
	return 0;
}


int cfgtool_commit_repository(Cfgtool *r) {
	if (!r->dirty)
		return 0;
	if (!cfgtool_is_locked(r))
		return -1;
	return file_write(r->value_file, r->da.data, r->da.used);
}


int cfgtool_close_repository(Cfgtool *r) {
	if (cfgtool_is_locked(r) && !cfgtool_keeping_lock(r))
		if (cfgtool_unlock(r) == -1)
			return -1;
	
	dynarr_free(&r->da);
	free(r->lock_file);
	free(r->repository);
	free(r->value_file);
	free(r);
	
	return 0;
}
