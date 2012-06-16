/*
 * File:	lockfile.h
 * Purpose:	Declarations for lock files.
 * Author:	Lars Wirzenius
 * Version:	$Id: lockfile.h,v 1.1 1996/10/17 07:38:13 liw Exp $
 */


int lockfile_create(const char *);
int lockfile_remove(const char *);
