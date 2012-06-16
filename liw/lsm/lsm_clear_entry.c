/*
 * lsm_clear_entry.c -- clear an LSM entry
 *
 * Lars Wirzenius
 * Part of Publib, see publib(3).
 * "@(#)publib-lsm:$Id: lsm_clear_entry.c,v 1.1 1995/11/26 09:54:58 liw Exp $"
 */
 
#include "publib/lsm.h"

void lsm_clear_entry(lsm_entry *e) {
	nntp_clear_article(&e->art);
}
