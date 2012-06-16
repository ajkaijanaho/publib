/*
 * File:	lockfile.h
 * Purpose:	Declarations for lock files.
 * Author:	Lars Wirzenius
 * Version:	$Id: lockfile.h,v 1.1.1.1 1996/11/05 21:06:56 liw Exp $
 */


int lockfile_create(const char *);
int lockfile_remove(const char *);
