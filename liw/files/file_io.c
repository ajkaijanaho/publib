/*
 * File:	file_io.c
 * Purpose:	File I/O.
 * Author:	Lars Wirzenius
 * Version:	"@(#)publib:$Id: file_io.c,v 1.1 1996/11/05 21:15:46 liw Exp $"
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "publib/alloc.h"
#include "publib/files.h"
#include "publib/errormsg.h"


#define MAX_TRIES 1024

static int create_save_temp_file(const char *, char *, int *);
static int copy_modes(const char *, const char *);
static int make_backup_name(char *, const char *, size_t);


int file_read_open(FILE *f, void **data, size_t *size) {
	struct dynarr da;
	int c;

	dynarr_init(&da, 1);
	while ((c = getc(f)) != EOF && !ferror(stdout)) {
		if (dynarr_resize(&da, da.used + 2) == -1) {
			__publib_error("out of memory");
			dynarr_free(&da);
			(void) fclose(f);
			return -1;
		}
		((char *) da.data)[da.used++] = c;
	}
	if (ferror(f)) {
		__publib_error("error reading from file");
		return -1;
	}

	*data = da.data;
	*size = da.used;

	return 0;
}


int file_read(const char *pathname, void **data, size_t *size) {
	FILE *f;
	int ret;

	f = fopen(pathname, "r");
	if (f == NULL) {
		__publib_error("error opening file for reading");
		return -1;
	}
	
	ret = file_read_open(f, data, size);
	if (fclose(f) == EOF) {
		__publib_error("error closing file after reading");
		ret = -1;
	}
	return ret;
}


int file_write(const char *pathname, void *data, size_t n) {
	FILE *f;
	int c;
	
	f = fopen(pathname, "w");
	if (f == NULL) {
		__publib_error("couldn't open file for writing");
		return -1;
	}
	(void) fwrite(data ? data : "", 1, n, f);
	c = ferror(f);
	if (fclose(f) == EOF || c) {
		__publib_error("error writing to file");
		return -1;
	}
	return 0;
}


int file_save(const char *name, void *data, size_t size, int keep_backup) {
	FILE *f;
	int c, fd;
	char temp_name[FILENAME_MAX];
	char back_name[sizeof(temp_name) + 4];

	if (strlen(name) + 64 > sizeof(temp_name)) {
		__publib_error("filename is too long (can't save)");
		return -1;
	}
		
	if (make_backup_name(back_name, name, sizeof(back_name)) == -1)
		return -1;

	if (create_save_temp_file(name, temp_name, &fd) == -1)
		return -1;
		
	f = fdopen(fd, "w");
	if (f == NULL) {
		__publib_error("error opening file");
		return -1;
	}
	
	(void) fwrite(data ? data : "", 1, size, f);

	c = ferror(f);
	if (fclose(f) == EOF || c) {
		__publib_error("error closing file");
		return -1;
	}
	
	if (rename(name, back_name) == -1 && errno != ENOENT) {
		__publib_error("rename of original failed (can't save)");
		return -1;
	}
	if (rename(temp_name, name) == -1) {
		__publib_error("rename of new failed (can't save)");
		return -1;
	}
	if (copy_modes(back_name, name) == -1)
		return -1;

	if (!keep_backup) {
		if (remove(back_name) == -1 && errno != ENOENT) {
			__publib_error("remove of backup failed (can't save)");
			return -1;
		}
	}

	return 0;
}


/**********************************************************************/


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
static int make_backup_name(char *back, const char *orig, size_t max) {
	if (strlen(orig) + 2 > max)
		return -1;

	sprintf(back, "%s~", orig);
	return 0;
}
