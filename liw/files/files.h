/*
 * files.h -- utility functions for files
 *
 * Part of publib.
 * "@(#)publib-files:$Id: files.h,v 1.2 1996/11/05 21:15:47 liw Exp $"
 */

#ifndef __publib_files_h
#define __publib_files_h

#include <stdio.h>	/* need FILE, size_t */

FILE *xfopen(const char *__filename, const char *__mode);
void xfclose(FILE *__fp);
void xfseek(FILE *__fp, long __offset, int __origin);

char *getaline(FILE *__fp);
char *xgetaline(FILE *__fp);

void fassert(FILE *__fp);


int file_read(const char *, void **, size_t *);
int file_read_open(FILE *, void **, size_t *);
int file_save(const char *, void *, size_t, int);
int file_write(const char *, void *, size_t);

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
