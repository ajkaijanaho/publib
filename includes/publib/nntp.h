/* Part of publib.

   Copyright (c) 2012 Antti-Juhani Kaijanaho.
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
 * nntp.h -- declarations for routines for NNTP clients
 *
 * Lars Wirzenius
 * Part of Publib.
 */

#ifndef __publib_nntp_h_included
#define __publib_nntp_h_included

#include <publib/alloc.h>
#include <stdio.h>
#include <time.h>

struct nntp_article {
	struct dynarr headers;
	struct dynarr body;
};

void nntp_init_article(struct nntp_article *);
void nntp_clear_article(struct nntp_article *);
int nntp_read_article_file(FILE *, struct nntp_article *);
int nntp_read_article_nntp(int, struct nntp_article *);
int nntp_write_article_file(FILE *, struct nntp_article *);
int nntp_write_article_nntp(int, struct nntp_article *);
int nntp_post_article(int, struct nntp_article *);

int nntp_add_header(struct nntp_article *, const char *);
int nntp_add_unique_header(struct nntp_article *, const char *, const char *);
int nntp_add_body(struct nntp_article *, const char *);
int nntp_get_header(char **, const struct nntp_article *, int);
int nntp_remove_header(struct nntp_article *, int);
int nntp_find_header(int *, const struct nntp_article *, int, const char *);
int nntp_copy_header(struct nntp_article *, struct nntp_article *, const char *);

int nntp_set_date(struct nntp_article *, time_t);

int nntp_get_article_by_id(int, struct nntp_article *, const char *);
int nntp_get_article_by_number(int, struct nntp_article *, int);
int nntp_get_current_article(int);

int nntp_open(int *, const char *, int);
int nntp_close(int);
int nntp_newnews(int, const char *, const char *, const char *, const char *);
int nntp_next_line(int, char **);
int nntp_post(int);
int nntp_write_next_line(int, const char *);
int nntp_write_end_line(int);
int nntp_group(int, const char *);
int nntp_goto_next_article(int);
void nntp_do_logs(int, int);

#endif
