/*
 * fname.h -- filename manipulation
 *
 * Part of publib.
 * "@(#)publib-fname:$Id: fname.h,v 1.1.1.1 1993/11/20 17:02:15 liw Exp $"
 */

#ifndef __publib_fname_h
#define __publib_fname_h

#include <stddef.h>	/* need size_t */

char *fnbase(const char *);
void fndelbeg(char *);
void fndelend(char *);
int fndelsuf(char *, const char *);
void fndir(char *, const char *);
int fnhome(char *, const char *);
void fnjoin(char *, const char *, const char *);
char *fnlastsuf(const char *);
int fnpathfind(const char *, const char *, char *, size_t);
size_t fnqualify(char *, const char *, size_t);
size_t fnsetsuf(char *, const char *, size_t);

#endif
