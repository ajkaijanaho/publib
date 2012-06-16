/*
 * files.h -- utility functions for files
 *
 * Part of publib.
 * "@(#)publib-files:$Id: files.h,v 1.1.1.1 1993/11/20 17:02:01 liw Exp $"
 */

#ifndef __publib_files_h
#define __publib_files_h

#include <stdio.h>	/* need FILE */

FILE *xfopen(const char *__filename, const char *__mode);
void xfclose(FILE *__fp);
void xfseek(FILE *__fp, long __offset, int __origin);

char *getaline(FILE *__fp);
char *xgetaline(FILE *__fp);

void fassert(FILE *__fp);

#if 0

/**/int copy_file(const char *__oldname, const char *__newname);
/**/int fcopy_file(FILE *__oldfile, FILE *__newfile);

/**/int move_file(const char *__oldname, const char *__newname);
/**/int fmove_file(FILE *__oldfile, FILE *__newfile);

/**/char *make_backup_file(const char *__fname);

/**/int eprintf(const char *, ...);
/**/int eprintf_register(void);

/**/int snprintf(char *, size_t, const char *, ...);

#endif

#endif
