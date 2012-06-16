/*
 * main.h -- declarations for main() utility functions
 *
 * Lars Wirzenius.
 * Part of Publib.  See publib(3).
 * "@(#)publib-main:$Id: main.h,v 1.2 1996/01/07 22:02:58 liw Exp $"
 */

#ifndef __publib_main_h_included
#define __publib_main_h_included

#include <stdio.h>

struct option;

int main_options(int, char **, struct option *);
int main_filter(int, char **, int (*)(FILE *, char *, void *), void *);

#endif
