/*
 * File: nntpart.c
 * Purpose: Routines for manipulating Usenet articles
 * Author: Lars Wirzenius
 * CVS: "@(#)publib:$Id: nntpart.c,v 1.4 1996/04/08 22:27:54 liw Exp $"
 */

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "publib/nntp.h"

#include "publib/nntp.h"
#include "publib/files.h"



void nntp_init_article(struct nntp_article *art) {
	dynarr_init(&art->headers, sizeof(char *));
	dynarr_init(&art->body, sizeof(char *));
}



void nntp_clear_article(struct nntp_article *art) {
	size_t i;
	char **p;

	for (p = art->headers.data, i = 0; i < art->headers.used; ++i)
		free(p[i]);
	for (p = art->body.data, i = 0; i < art->body.used; ++i)
		free(p[i]);
	art->headers.used = 0;
	art->body.used = 0;
}



int nntp_add_header(struct nntp_article *art, const char *line) {
	char *p;

	if (art->headers.used == art->headers.alloc)
		if (dynarr_resize(&art->headers, art->headers.used + 1) == -1)
			return -1;

	p = strdup(line);
	if (p == NULL)
		return -1;

	((char **)art->headers.data)[art->headers.used] = p;
	++art->headers.used;

	return 0;
}



int nntp_add_body(struct nntp_article *art, const char *line) {
	char *p;

	if (art->body.used == art->body.alloc)
		if (dynarr_resize(&art->body, art->body.used + 1) == -1)
			return -1;

	p = strdup(line);
	if (p == NULL)
		return -1;

	((char **)art->body.data)[art->body.used] = p;
	++art->body.used;

	return 0;
}



int nntp_add_unique_header(struct nntp_article *art, const char *hdr, const char *s)
{
	char **p, *buf;
	int i, j, ret;

	p = art->headers.data;
	if (nntp_find_header(&i, art, 0, hdr) != -1) {
		do {
			for (j = i; j < art->headers.used; ++j)
				p[j] = p[j+1];
			--art->headers.used;
		} while (i < art->headers.used && isspace(p[i][0]));
	}

	buf = malloc(strlen(hdr) + 2 + strlen(s) + 2);
	if (buf == NULL)
		return -1;
	sprintf(buf, "%s: %s", hdr, s);
	ret = nntp_add_header(art, buf);
	free(buf);

	return ret;
}



int nntp_read_article_file(FILE *f, struct nntp_article *art) {
	char *line;

	assert(f != NULL);
	assert(art != NULL);

	nntp_init_article(art);

	while ((line = getaline(f)) != NULL && *line != '\0') {
		if (nntp_add_header(art, line) == -1)
			return -1;
		free(line);
	}

	while ((line = getaline(f)) != NULL) {
		if (nntp_add_body(art, line) == -1)
			return -1;
		free(line);
	}

	return 0;
}



int nntp_post_article(int fd, struct nntp_article *art) {
	char **p;
	int i;

	assert(fd > 0);
	assert(art != NULL);

        if (nntp_post(fd) == -1)
                return -1;

	p = art->headers.data;
	for (i = 0; i < art->headers.used; ++i)
                if (nntp_write_next_line(fd, p[i]) == -1)
			return -1;

	if (nntp_write_next_line(fd, "") == -1)
		return -1;

	p = art->body.data;
	for (i = 0; i < art->body.used; ++i)
                if (nntp_write_next_line(fd, p[i]) == -1)
			return -1;

        if (nntp_write_end_line(fd) == -1)
                return -1;

	return 0;
}



int nntp_write_article_file(FILE *f, struct nntp_article *art) {
	char **p;
	int i;

	assert(f != NULL);
	assert(art != NULL);

	p = art->headers.data;
	for (i = 0; i < art->headers.used; ++i)
                if (fprintf(f, "%s\n", p[i]) == EOF)
			return -1;

	if (fprintf(f, "\n") == EOF)
		return -1;

	p = art->body.data;
	for (i = 0; i < art->body.used; ++i)
                if (fprintf(f, "%s\n", p[i]) == -1)
			return -1;

	if (fprintf(f, "\n") == EOF)
                return -1;

	return 0;
}



int nntp_find_header(int *j, const struct nntp_article *art, int i, 
const char *hdr) {
	char **p;
	size_t hdr_len;

	assert(art != NULL);
	assert(j != NULL);
	assert(i >= 0);
	assert(hdr != NULL);
	assert(*hdr != '\0');
	assert(strchr(hdr, ':') == NULL);

	hdr_len = strlen(hdr);
	for (p = art->headers.data; i < art->headers.used; ++i) {
		/* FIXME: the comparison below should be case _in_sensitive */
		if (strncmp(p[i], hdr, hdr_len) == 0 && p[i][hdr_len] == ':') {
			*j = i;
			return 0;
		}
	}
	return -1;
}



int nntp_get_header(char **p, const struct nntp_article *art, int i) {
	assert(p != NULL);
	assert(art != NULL);
	assert(i >= 0);

	if (i >= art->headers.used)
		return -1;
	*p = ((char **) art->headers.data)[i];
	return 0;
}



void nntp_dump_article(FILE *f, struct nntp_article *art) {
	char **p;
	int i;

	fprintf(f, "dumping article:---------------------\n");
	for (p = art->headers.data, i = 0; i < art->headers.used; ++i)
		fprintf(f, "%s\n", p[i]);
	fprintf(f, "-------------------------------------\n");
	for (p = art->body.data, i = 0; i < art->body.used; ++i)
		fprintf(f, "%s\n", p[i]);
	fprintf(f, "done---------------------------------\n");
}


int nntp_set_date(struct nntp_article *art, time_t date) {
        static const char * const weekday[] = {
                "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
        };
        static const char * const month[] = {
                "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
        };
        char buf[1024];
        struct tm *tm;

        assert(art != NULL);

        tm = gmtime(&date);
        sprintf(buf, "%s, %02d %s %02d %02d:%02d:%02d GMT",
                weekday[tm->tm_wday], tm->tm_mday, month[tm->tm_mon],
                tm->tm_year, tm->tm_hour, tm->tm_min, tm->tm_sec);

        if (nntp_add_unique_header(art, "Date", buf) == -1)
                return -1;
        return 0;
}
