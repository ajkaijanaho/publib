/*
 * sbuf_io.c -- reading and writing buffers and marks
 *
 * Part of Publib.  See manpage for more information.
 * "@(#)publib-sbuf:$Id: sbuf_io.c,v 1.6 1996/11/24 13:41:54 liw Exp $"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#include "publib/sbuf.h"
#include "publib/errormsg.h"


#define IO_BUFFER_SIZE 8*1024
#define MAX_TRIES 1024


static int create_save_temp_file(const char *, char *, int *);
static int copy_modes(const char *, const char *);
static int make_backup_name(char *, char *, size_t);


int sbuf_load(Sbuf *buf) {
	FILE *f;
	int ret;

	sbuf_set_flags(buf, SBUF_LOADED_FLAG);
	ret = 0;

	if (buf->name == NULL)
		goto ok;

	sbuf_remark(buf->aux, 0, sbuf_length(buf));
	if (sbuf_strchange(buf->aux, "", 0) == -1) {
		__publib_error("error clearing old contents of file");
		return -1;
	}

	f = fopen(buf->name, "r");
	if (f == NULL) {
		if (errno == ENOENT)
			goto ok;
		__publib_error("error loading file (no permission?)");
		return -1;
	}

	ret = sbuf_insert_file(buf->aux, f);

	if (fclose(f) == EOF) {
		__publib_error("error closing file while loading buffer");
		ret = -1;
	}

ok:
	sbuf_clear_flags(buf, SBUF_DIRTY_FLAG);
	return ret;
}


int sbuf_save(Sbuf *buf, int keep_backup) {
	FILE *f;
	int e, fd, ret;
	char temp_name[FILENAME_MAX];
	char back_name[sizeof(temp_name) + 4];

	if (buf->name == NULL) {
		__publib_error("file has no name (can't save)");
		return -1;
	}

	if (strlen(buf->name) + 64 > sizeof(temp_name)) {
		__publib_error("filename is too long (can't save)");
		return -1;
	}
		
	if (make_backup_name(back_name, buf->name, sizeof(back_name)) == -1) {
		__publib_error("backup name too long");
		return -1;
	}

	if (create_save_temp_file(buf->name, temp_name, &fd) == -1)
		return -1;
		
	f = fdopen(fd, "w");
	if (f == NULL) {
		__publib_error("error opening file");
		return -1;
	}

	sbuf_remark(buf->aux, 0, sbuf_length(buf));
	ret = sbuf_write_to_file(buf->aux, f);
	e = errno;

	if (fclose(f) == EOF) {
		if (ret == 0) {
			e = errno;
			__publib_error("error closing file");
		}
		ret = -1;
	}

	if (rename(buf->name, back_name) == -1 && errno != ENOENT) {
		__publib_error("rename of original failed (can't save)");
		return -1;
	}
	if (rename(temp_name, buf->name) == -1) {
		__publib_error("rename of new failed (can't save)");
		return -1;
	}
	if (copy_modes(back_name, buf->name) == -1)
		return -1;

	if (!keep_backup) {
		if (remove(back_name) == -1 && errno != ENOENT) {
			__publib_error("remove of backup failed (can't save)");
			return -1;
		}
	}

	if (ret == 0)
		sbuf_set_dirty(buf, 0);
	else
		errno = e;

	return ret;
}


int sbuf_insert_file(Sbufmark *mark, FILE *f) {
	char iobuf[IO_BUFFER_SIZE];
	size_t n;
	long begin;
	
	begin = sbuf_mark_begin(mark);
	while ((n = fread(iobuf, 1, IO_BUFFER_SIZE, f)) > 0 && !ferror(f)) {
		sbuf_remark(mark, sbuf_mark_end(mark), 0);
		if (sbuf_strchange(mark, iobuf, n) == -1)
			goto error;
	}
	if (!ferror(f))
		return 0;
	/*FALLTHROUGH*/
	__publib_error("error reading file");
error:
	sbuf_remark(mark, begin, sbuf_mark_end(mark) - begin);
	(void) sbuf_strchange(mark, "", 0);
	return -1;
}



int sbuf_write_to(Sbufmark *mark, const char *filename) {
	FILE *f;
	int ret, e;

	f = fopen(filename, "w");
	if (f == NULL) {
		__publib_error("couldn't open file for writing");
		return -1;
	}

	e = 0;
	ret = sbuf_write_to_file(mark, f);
	if (ret == -1) {
		e = errno;
		ret = -1;
	}
	
	if (fclose(f) == EOF) {
		if (e == 0) 
			e = errno;
		ret = -1;
	}

	errno = e;
	return ret;
}


int sbuf_write_to_file(Sbufmark *m, FILE *f) {
	char iobuf[IO_BUFFER_SIZE];
	char *p;
	long i, n, nn, pos;
	Sbuf *buf;

	n = sbuf_mark_length(m);
	pos = sbuf_mark_begin(m);
	buf = m->buf;

	while (n > 0) {
		nn = (n < IO_BUFFER_SIZE) ? n : IO_BUFFER_SIZE;
		n -= nn;
		for (i = 0, p = iobuf; i < nn; ++i, ++p, ++pos)
			*p = sbuf_charat(buf, pos);
		if (fwrite(iobuf, nn, 1, f) != 1) {
			__publib_error("error writing to file");
			return -1;
		}
	}

	return 0;
}


/**********************************************************************
 * Local functions.
 */


/*
 * Function:	create_save_temp_file
 * Purpose:	Create a new file in same directory as the file to be saved.
 * Arguments:	original	the name of the original file
 *		buf		where the new file's name is stored
 *		fd		where file descriptor of new file is stored
 * Return:	-1 for failure, 0 for success.
 * Note:	buf should be big enough; strlen(original) + 64 should
 *		be enough (unless you have _really_ big unsigned longs).
 */
static int create_save_temp_file(const char *original, char *buf, int *fd) {
	char *p;
	unsigned long i;
	time_t random;

	strcpy(buf, original);
	p = strrchr(buf, '/');
	if (p != NULL)
		p[1] = '\0';
	else
		buf[0] = '\0';

	time(&random);
	*fd = -1;
	p = strchr(buf, '\0');
	for (i = 0; i < MAX_TRIES && *fd == -1; ++i) {
		sprintf(p, "#%lu-%lu#", (unsigned long) random, i);
		*fd = open(buf, O_WRONLY | O_CREAT | O_EXCL, 0600);
		if (*fd == -1 && errno != EEXIST) {
			__publib_error("can't create temporary file");
			return -1;
		}
	}

	if (*fd == -1)
		return -1;
	return 0;
}



/*
 * Function:	copy_modes
 * Purpose:	Copy the permissions from one file to another.
 * Arguments:	src	original file
 *		tgt	target file
 * Return:	-1 for error, 0 for OK.
 */
static int copy_modes(const char *src, const char *tgt) {
	struct stat st;
	
	if (stat(src, &st) == -1) {
		if (errno == ENOENT)
			return 0;
		__publib_error("Can't stat file (can't complete save)");
		return -1;
	}
	if (chmod(tgt, st.st_mode) == -1) {
		__publib_error("Can't chmod file (can't complete save)");
		return -1;
	}
	return 0;
}



/*
 * Function:	make_backup_name
 * Purpose:	Create name of backup file from original file.
 * Note:	The target buffer must be big enough.
 */
static int make_backup_name(char *back, char *orig, size_t max) {
	if (strlen(orig) + 2 > max)
		return -1;

	sprintf(back, "%s~", orig);
	return 0;
}
