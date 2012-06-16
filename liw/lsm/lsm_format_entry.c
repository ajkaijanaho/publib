/*
 * lsm_format_entry.c -- format an LSM entry for textual display
 *
 * Lars Wirzenius
 * Part of Publib, see publib(3).
 * "@(#)publib-lsm:$Id: lsm_format_entry.c,v 1.2 1996/01/07 22:00:04 liw Exp $"
 */
 
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

#include "publib/strutil.h"
#include "publib/lsm.h"

static int find_break(char *line, int w) {
	int i, minw;
	
	i = strlen(line);
	if (i <= w)
		return i;
	if (isspace(line[w]))
		return w;
	minw = w/2;
	for (i = w; i > minw && !isspace(line[i]); --i)
		continue;
	if (i <= minw)
		return w;
	return i;
}

static int add_to_lines(char *p, char **tab, int *n, int max, int indent, int w) {
	char buf[1024*8];
	const int tabsize = 8;
	int i, do_indent, brk;

	if (*n == max) {
		lsm_set_error(0, 0, "lsm_format_entry: not enough room for output");
		return -1;
	}
	
	if (strlen(p) * tabsize > sizeof(buf)) {
		lsm_set_error(0, 0, "lsm_format_entry: line too long after tab "
				"conversion");
		return -1;
	}
	
	strcpy(buf, p);
	struntabify(buf, tabsize);
	
	if (!isspace(buf[0]) && (p = strchr(buf, ':')) != NULL) {
		++p;
		while (isspace(*p))
			strdel(p, 1);
		while (p - buf < indent)
			strins(p++, " ");
		do_indent = 0;
	} else
		do_indent = 1;
	
	do {
		if (do_indent) {
			strltrim(buf);
			for (i = 0; i < indent; ++i)
				strins(buf, " ");
		}
		brk = find_break(buf, w);
		tab[*n] = strndup(buf, brk);
		if (tab[*n] == NULL) {
			lsm_set_error(0, errno, "lsm_format_entry: out of memory");
			return -1;
		}
		++(*n);
		strdel(buf, brk);
		do_indent = 1;
	} while (buf[0] != '\0');
	
	return 0;
}

static void free_lines(char **tab, int n) {
	while (--n >= 0)
		free(tab[n]);
}

int lsm_format_entry(lsm_entry *e, char **tab, int max, int indent, int width) {
	int i, n;
	char *p;
	
	n = 0;
	for (i = 0; (p = lsm_get_line(e, i)) != NULL; ++i) {
		if (add_to_lines(p, tab, &n, max, indent, width) == -1) {
			free_lines(tab, n);
			return -1;
		}
	}
	
	return n;
}
