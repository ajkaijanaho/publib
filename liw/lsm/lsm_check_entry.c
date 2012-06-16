/*
 * lsm_check_entry.c -- check an LSM entry for correctness
 *
 * Lars Wirzenius
 * Part of Publib, see publib(3).
 * "@(#)publib-lsm:$Id: lsm_check_entry.c,v 1.9 1996/04/08 22:28:35 liw Exp $"
 */
 
#include "publib/lsm-config.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "publib/cmp.h"
#include "publib/strutil.h"
#include "publib/lsm.h"

#if !HAVE_STRNCASECMP
#define strncasecmp(a,b,n) strncmp(a,b,n)
#endif


static int field_names(const lsm_entry *);
static int emails_are_correct(const lsm_entry *);
static int dates_are_correct(const lsm_entry *);
static int sites_are_correct(const lsm_entry *);

static int check_set_of_headers(const lsm_entry *, char **, int,
	int (*)(char *, char *, int));
static int in_list(char *, char **, int);
static char *get_header_name(char *, int);
static int check_email(char *, char *, int);
static int check_date(char *, char *, int);
static int date_is_valid(char *);


int lsm_check_entry(const lsm_entry *e) {
	static int (*funtab[])(const lsm_entry *) = {
		field_names,
		emails_are_correct,
		dates_are_correct,
		sites_are_correct,
	};
	int i, ret;
	
	ret = 0;
	for (i = 0; i < sizeof(funtab) / sizeof(*funtab); ++i)
		if (funtab[i](e) == -1)
			ret = -1;
	return ret;
}


static int field_names(const lsm_entry *e) {
	static char *req[] = {
		"Title",
		"Version",
		"Description",
		"Primary-site",
		"Entered-date",
	};
	static int nreq = sizeof(req) / sizeof(char *);
	static char *opt[] = {
		"Keywords",
		"Author",
		"Maintained-by",
		"WWW-site",
		"Alternate-site",
		"Original-site",
		"Platforms",
		"Copying-policy",
		"Checked-status",
		"Checked-date",
	};
	static int nopt = sizeof(opt) / sizeof(char *);
	int i, j, before_first_keyword, ret;
	char *p, *q;
	
	qsort(req, nreq, sizeof(char *), cmp_charptr);
	qsort(opt, nopt, sizeof(char *), cmp_charptr);

	ret = 0;
	before_first_keyword = 1;
	for (i = 0; nntp_get_header(&p, &e->art, i) == 0; ++i) {
		if (*p == '\0' || isspace(*p)) {
			if (before_first_keyword) {
				lsm_set_error(i+1, 0, "this text line is before "
						      "the first line with a "
      						      "keyword");
      				ret = -1;
			}
			continue;
		}
		q = get_header_name(p, i+1);
		before_first_keyword = before_first_keyword && (q == NULL);
		if (q == NULL)
			ret = -1;
		else if (!in_list(q, req, nreq) && !in_list(q, opt, nopt)) {
			lsm_set_error(i+1, 0, "unknown keyword `%s'", q);
			ret = -1;
			continue;
		}
	}
	
	for (i = 0; i < nreq; ++i) {
		if (nntp_find_header(&j, &e->art, 0, req[i]) == -1) {
			lsm_set_error(0, 0, "required header `%s' is missing",
				req[i]);
			ret = -1;
		}
	}
	
	return ret;
}


static int in_list(char *header, char **list, int listlen) {
	return bsearch(&header, list, listlen, sizeof(char *), cmp_charptr) != NULL;
}

static char *get_header_name(char *line, int lineno) {
	static char buf[32];
	char *colon;
	
	colon = strchr(line, ':');
	if (colon == NULL) {
		lsm_set_error(lineno, 0, "no keyword (= no colon) on this line");
		lsm_set_error(lineno, 0, "  (lines beginning at column 1 must "
			" begin with a keyword)");
		return NULL;
	}
	if (colon > line && isspace(colon[-1])) {
		lsm_set_error(lineno, 0, "no whitespace allowed between keyword "
			" and colon (sorry)");
		return NULL;
	}
	if (colon - line >= sizeof(buf)) {
		lsm_set_error(lineno, 0, "keyword on this line is too long");
		lsm_set_error(lineno, 0, "  (therefore, it isn't one of the "
			" valid ones)");
		return NULL;
	}
	strmaxcpy(buf, line, colon - line);
	return buf;
}


static int emails_are_correct(const lsm_entry *e) {
	static char *email_headers[] = {
		"Author",
		"Maintained-by",
	};
	static int n = sizeof(email_headers) / sizeof(char *);
	
	return check_set_of_headers(e, email_headers, n, check_email);
}

static int check_email(char *s, char *hdr, int line) {
#if CHECK_EMAIL
	if (strchr(s, '@') == NULL) {
		lsm_set_error(line, 0, "field `%s' does not seem to contain", hdr);
		lsm_set_error(line, 0, "  an e-mail address (no `@' character)");
		return -1;
	}
#endif
	return 0;
}


static int dates_are_correct(const lsm_entry *e) {
	static char *date_headers[] = {
		"Entered-date",
		"Checked-date",
	};
	static int n = sizeof(date_headers) / sizeof(char *);
	return check_set_of_headers(e, date_headers, n, check_date);
}

static int check_date(char *s, char *hdr, int line) {
	s = strchr(s, ':');
	if (s == NULL) {
		lsm_set_error(line, 0,
			"internal error: line should have contained colon");
		return -1;
	}
	++s;
	if (date_is_valid(s))
		return 0;
	lsm_set_error(line, 0, "field `%s' contains invalid date `%s'", hdr, s);
	lsm_set_error(line, 0, "  (correct format: ddMMMyy, where dd is two digit");
	lsm_set_error(line, 0, "  day of month, MMM is first three letters of");
	lsm_set_error(line, 0, "  the English name of the month, and yy is the");
	lsm_set_error(line, 0, "  last two digits of the year in the Gregorian");
	lsm_set_error(line, 0, "  calendar).");
	return -1;
}

static int date_is_valid(char *s) {
	int dd, mm, yy, leap;
	static const char *months[12] = {
		"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
		"JUL", "AUG", "SEP", "OCT", "NOV", "DEC",
	};
	static const int monlen[2][12] = {
		{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
		{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	};

	if (s == NULL)
		return 1;

	while (isspace(*s))
		++s;

	if (!isdigit(s[0]) || !isdigit(s[1]))
		return 0;
	if (!isdigit(s[5]) || !isdigit(s[6]))
		return 0;
	if (!isspace(s[7]) && s[7] != '\0')
		return 0;
	dd = 10*(s[0]-'0') + (s[1]-'0');
	yy = 10*(s[5]-'0') + (s[6]-'0') + 1900;

	for (mm = 0; mm < 12; ++mm)
		if (strncasecmp(s+2, months[mm], 3) == 0)
			break;
	if (mm == 12)
		return 0;

	leap = (yy % 4) == 0 && ((yy % 100) != 0 || (yy % 400) == 0);
	if (mm < 0 || mm > 12 || dd < 0 || dd > monlen[leap][mm])
		return 0;

	return 1;
}


static int sites_are_correct(const lsm_entry *e) {
	return 0;
}



static int check_set_of_headers(const lsm_entry *e, char **list, int n, 
int (*fun)(char *, char *, int)) {
	int i, j, first, ret;
	char *p;
	
	ret = 0;
	for (i = 0; i < n; ++i) {
		if (nntp_find_header(&j, &e->art, 0, list[i]) == 0) {
			first = 1;
			for (;;) {
				if (nntp_get_header(&p, &e->art, j++) == -1)
					break;
				if (!first && !isspace(*p))
					break;
				first = 0;
				if (fun(p, list[i], j) == -1)
					ret = -1;
			}
		}
	}
	return ret;
}
