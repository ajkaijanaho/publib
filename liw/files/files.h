/* Part of publib.

   Copyright (c) 1994-2006 Lars Wirzenius.  All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
