/*
 * lsm.h -- declarations for Linux Software Map routines
 *
 * Lars Wirzenius
 * Part of Publib, see publib(3).
 * "@(#)publib-lsm:$Id: lsm.h,v 1.3 1996/04/08 22:28:34 liw Exp $"
 */
 


#ifndef __publib_lsm_h_included
#define __publib_lsm_h_included

#include <stdio.h>
#include <publib/nntp.h>

typedef struct {
	struct nntp_article art;
} lsm_entry;

void lsm_init_entry(lsm_entry *);
void lsm_clear_entry(lsm_entry *);
int lsm_read_entry(FILE *, long *, lsm_entry *);
int lsm_write_entry(FILE *, const lsm_entry *);
int lsm_check_entry(const lsm_entry *);
char *lsm_get_line(const lsm_entry *, int);
int lsm_format_entry(lsm_entry *, char **, int, int, int);
char *lsm_error(void);
void lsm_set_error(long, int, const char *, ...);

#endif
