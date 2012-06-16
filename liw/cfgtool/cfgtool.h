/*
 * File:	cfgtool.h
 * Purpose:	Declarations for library routines handling Debian's cfgtool.
 * Author:	Lars Wirzenius
 * Version:	"@(#)publib-cfgtool:$Id: cfgtool.h,v 1.2 1996/11/23 06:20:22 liw Exp $"
 */

#ifndef __publib_cfgtool_h_included
#define __publib_cfgtool_h_included

#include <stdio.h>	/* need size_t and FILE */

typedef struct cfgtool_repository Cfgtool;

Cfgtool *cfgtool_init_repository(void);
int cfgtool_name_repository(Cfgtool *, const char *);
int cfgtool_create_disk_repository(Cfgtool *);
int cfgtool_load_repository(Cfgtool *);
int cfgtool_commit_repository(Cfgtool *);
int cfgtool_close_repository(Cfgtool *);
int cfgtool_repository_is_dirty(Cfgtool *);
int cfgtool_repository_is_loaded(Cfgtool *);

int cfgtool_lock(Cfgtool *);
int cfgtool_unlock(Cfgtool *);
void cfgtool_assume_lock(Cfgtool *);
void cfgtool_keep_lock(Cfgtool *, int);
int cfgtool_is_locked(Cfgtool *);
int cfgtool_keeping_lock(Cfgtool *);

int cfgtool_create(Cfgtool *, const char *, const void *, size_t);
int cfgtool_destroy(Cfgtool *, const char *);
int cfgtool_exists(Cfgtool *, const char *);
int cfgtool_ith_name(Cfgtool *, int, char **);
int cfgtool_set(Cfgtool *, const char *, const void *, size_t);
int cfgtool_get(Cfgtool *, const char *, void **, size_t *);
int cfgtool_is_true(Cfgtool *, const char *);
int cfgtool_is_false(Cfgtool *, const char *);

int cfgtool_write_all(Cfgtool *, FILE *);
int cfgtool_read_all(Cfgtool *, FILE *, int);

#ifdef __publib__

	#include <publib/alloc.h>
	struct cfgtool_repository {
		struct dynarr da;
		char *lock_file;
		char *repository;
		char *value_file;
		unsigned loaded:1;
		unsigned dirty:1;
		unsigned have_lock:1;
		unsigned keep_lock:1;
	};

#endif

#endif
