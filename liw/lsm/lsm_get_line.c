/*
 * lsm_get_line.c -- get the ith line of an entry
 *
 * Lars Wirzenius
 * Part of Publib, see publib(3).
 * "@(#)publib-lsm:$Id: lsm_get_line.c,v 1.2 1996/04/08 22:28:35 liw Exp $"
 */
 
#include "publib/lsm.h"

char *lsm_get_line(const lsm_entry *e, int i) {
	char *p;
	if (nntp_get_header(&p, &e->art, i) == -1)
		return NULL;
	return p;
}
