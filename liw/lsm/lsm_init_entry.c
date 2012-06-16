/*
 * lsm_init_entry.c -- initialize an LSM entry
 *
 * Lars Wirzenius
 * Part of Publib, see publib(3).
 * "@(#)publib-lsm:$Id$"
 */
 
#include "publib/lsm.h"

void lsm_init_entry(lsm_entry *e) {
	nntp_init_article(&e->art);
}
