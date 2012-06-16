#ifndef __publib_strutil_h
#define __publib_strutil_h

#include <stddef.h>	/* need size_t */

void *memarrfill(void *, size_t, size_t);
void *memdel(void *, size_t, size_t);
void *memfill(void *, size_t, const void *, size_t);
void *memins(void *, size_t, const void *, size_t);
void memisort(void *, size_t, size_t, const void *,
		int (*)(const void *, const void *));
void *memmem(const void *, size_t, const void *, size_t);
int memoverlap(const void *, size_t, const void *, size_t);
void *memrchr(const void *, int, size_t);
void *memrev(void *, size_t, size_t);
void *memrmem(const void *, size_t, const void *, size_t);
void memswap(void *, void *, size_t);
void *memshuffle(void *, size_t, size_t);

char *stracat(const char *, ...);
char *stranaxfrm(char *);
char *strchange(char *, size_t, const char *, size_t);
void strcins(char *, int);
void strcstr(char *, size_t, const void *, size_t);
char *strdel(char *, size_t);
size_t strdiff(const char *, const char *);
char *strend(const char *);
int strendzap(char *, const char *);
int strgsub(char *, const char *, const char *, size_t);
unsigned long strhash(const char *);
char *strinit(char *, int, size_t);
char *strins(char *, const char *);
char *strltrim(char *);
char *strnins(char *, const char *, size_t);
size_t strnlen(const char *, size_t);
char *strmaxcpy(char *, const char *, size_t);
char *strmove(char *, const char *);
char *strmtrim(char *);
char *strndup(const char *, size_t);
int stroverlap(const char *, const char *);
char *strrev(char *);
char *strright(const char *, size_t);
char *strrot13(char *);
char *strrstr(const char *, const char *);
char *strset(char *, int);
int strsplit(char *, char **, int, const char *);
char *strsub(char *, const char *, const char *);
char *strrtrim(char *);
char *strtrim(char *);
char *strshuffle(char *);
char *strtabify(char *, size_t);
void strtrexpand(char *, const char *);
void struncstr(void *, const char *, size_t);
char *struntabify(char *, size_t);
int strvars(char **, const char *, char *(*)(const char *));
int strzap(char *, const char *);

/* new */

#if 0

char *strmany(char *, const char *, size_t);

unsigned long memsum(const void *buf, size_t n) {
	assert(buf != NULL);
	assert(0);
	/* This is as yet unwritten.  I will need to find a good source
	   for how the checksum is implemented in for example sum(1).
	   I might be able to use GNU's or BSD's versions, but will have
	   to look into licensing. */
	return 0;
}

int strvercmp(const void *, const void *);


int strgetver(const char *, int *, int *, int *);

int strgetver(const char *version, int *major, int *minor, int *patchlevel) {
	char *p;

	assert(version != NULL);
	assert(major != NULL);
	assert(minor != NULL);
	assert(patchlevel != NULL);

	*major = (int) strtol(version, &p, 10);
	*minor = (int) strtol(p, &p, 10);
	*patchlevel = (int) strtol(p, &p, 10);
	if (*p != '\0' || p == version)
		return -1;
	if (*major < 0 || *minor < 0 || *patchlevel < 0)
		return -1;
	return 0;
}


int strsoundex(const char *);


#endif

#endif
