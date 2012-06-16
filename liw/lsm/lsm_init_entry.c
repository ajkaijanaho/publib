/*
 * lsm_init_entry.c -- initialize an LSM entry
 *
 * Lars Wirzenius
 * Part of Publib, see publib(3).
 * "@(#)publib-lsm:$Id: lsm_init_entry.c,v 1.1 1995/11/26 09:54:59 liw Exp $"
 */
 
#include "publib/lsm.h"

void lsm_init_entry(lsm_entry *e) {
	nntp_init_article(&e->art);
}
