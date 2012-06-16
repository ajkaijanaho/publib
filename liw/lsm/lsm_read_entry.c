/*
 * lsm_read_entry.c -- read/write an LSM entry
 *
 * Lars Wirzenius
 * Part of Publib, see publib(3).
 * "@(#)publib-lsm:$Id: lsm_read_entry.c,v 1.4 1996/04/08 22:28:35 liw Exp $"
 */
 
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "publib/files.h"
#include "publib/lsm.h"



/*
 * Lines that look like these delimit the LSM entry (in the current format).
 */
#define BEGIN_MARKER	"Begin3"
#define END_MARKER	"End"



static int read_line(FILE *, long *, char **);
static int skip_to_begin(FILE *, long *);
static int read_to_end(FILE *, long *, lsm_entry *);



int lsm_read_entry(FILE *f, long *lineno, lsm_entry *e) {
	int ret;
	
	ret = skip_to_begin(f, lineno);
	if (ret <= 0)
		return ret;
	return read_to_end(f, lineno, e);
}



int lsm_write_entry(FILE *f, const lsm_entry *e) {
	int i;
	char *p;

	if (fprintf(f, "%s\n", BEGIN_MARKER) < 0)
		return -1;
	for (i = 0; (p = lsm_get_line(e,i)) != NULL; ++i)
		if (fprintf(f, "%s\n", p) < 0)
			return -1;
	if (fprintf(f, "%s\n", END_MARKER) < 0)
		return -1;
	return 0;
}



/*
 * Function: read_line
 * Purpose: Read the next complete line.
 * Arguments:
 *	f	the file from which to read
 *	lineno	pointer to line number (initial value: number of _previous_ line)
 *		read_line increments the line number
 *	line	pointer to the char* that will be set to the beginning of the
 *		line
 * Description: read_line reads the next complete line from file `f', allocates
 *	memory for it dynamically, and returns a pointer to the line via
 *	the `line' argument.
 * Return: -1 for error, 0 for end of file, 1 for line was read normally.
 */
static int read_line(FILE *f, long *lineno, char **line) {
	*line = getaline(f);
	if (*line == NULL) {
		if (ferror(f)) {
			lsm_set_error(-1, errno, "read error");
			return -1;
		}
		return 0;
	}
	++(*lineno);
	return 1;
}



/*
 * Read lines until the beginning marker is read.
 */
static int skip_to_begin(FILE *f, long *lineno) {
	char *line;
	int found, ret;
	const size_t marker_length = strlen(BEGIN_MARKER);

	found = 0;
	while (!found && (ret = read_line(f, lineno, &line)) > 0) {
		found = (strncmp(line, BEGIN_MARKER, marker_length) == 0);
		free(line);
	}

	if (ret == -1)
		return -1;
	if (!found)
		return 0;
	return 1;
}



/*
 * Read lines until the end marker is read, and append lines to the entry.
 * Return -1 for error, 0 for end of file, 1 for an entry was read successfully.
 */
static int read_to_end(FILE *f, long *lineno, lsm_entry *e) {
	char *line;
	int nonempty;
	const size_t marker_length = strlen(END_MARKER);

	nonempty = 0;
	for (;;) {
		switch (read_line(f, lineno, &line)) {
		case -1:
			return -1;
		case 0:
			return nonempty;
		default:
			nonempty = 1;
			if (strncmp(line, END_MARKER, marker_length) == 0)
				return 1;
			if (nntp_add_header(&e->art, line) == -1) {
				lsm_set_error(-1, errno, "memory allocation error");
				free(line);
				return -1;
			}
			free(line);
			break;
		}
	}
	/*NOTREACHED*/
}
