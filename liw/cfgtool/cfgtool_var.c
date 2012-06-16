/*
 * File:	var.c
 * Purpose:	Implementation of library routines handling Debian's cfgtool.
 * Author:	Lars Wirzenius
 * Version:	"@(#)cfgtool:$Id: cfgtool_var.c,v 1.2 1996/11/23 06:20:44 liw Exp $"
 */

#include <stdlib.h>
#include <string.h>

#include "publib/files.h"
#include "publib/strutil.h"
#include "publib/alloc.h"
#include "publib/errormsg.h"
#include "publib/base64.h"
#include "publib/cfgtool.h"


/*
 * Type:	byte
 * Purpose:	A bit of a mouthful.
 */
typedef unsigned char byte;


/*
 * Type:	Var
 * Purpose:	In-memory representation of variable in repository.
 */
typedef struct {
	byte *name;
	size_t namelen;
	byte *value;
	size_t len;
	byte *next;
} Var;



static int parse_variable(byte *, Var *);
static int find_next_variable(byte **, Cfgtool *, Var *);
static int find_variable(const char *, Cfgtool *, Var *);
static void destroy_variable(const char *, Cfgtool *);
static char *decode_variable(Var *, size_t *);
static int put_variable(const char *, Cfgtool *, const byte *, size_t);



int cfgtool_create(Cfgtool *r, const char *var, const void *value, 
size_t size) {
	if (cfgtool_exists(r, var))
		return -1;
	return cfgtool_set(r, var, value, size);
}


int cfgtool_destroy(Cfgtool *r, const char *var) {
	destroy_variable(var, r);
	return 0;
}


int cfgtool_exists(Cfgtool *r, const char *var) {
	Var v;
	return find_variable(var, r, &v);
}


int cfgtool_set(Cfgtool *r, const char *var, const void *value,
size_t size) {
	return put_variable(var, r, value, size);
}


int cfgtool_get(Cfgtool *r, const char *var, void **value, 
size_t *size) {
	Var v;

	if (find_variable(var, r, &v) == -1)
		return -1;
	*value = decode_variable(&v, size);
	if (*value == NULL)
		return -1;
	return 0;
}


int cfgtool_is_true(Cfgtool *r, const char *var) {
	Var v;
	
	if (find_variable(var, r, &v) == -1)
		return -1;
	if (strncmp(v.value, "5V2c", 4) == 0)	/* 5V2c is base64 for yes */
		return 0;
	return -1;
}


int cfgtool_is_false(Cfgtool *r, const char *var) {
	return -!cfgtool_is_true(r, var);
}


int cfgtool_ith_name(Cfgtool *r, int i, char **name) {
	Var v;
	byte *loc;

	loc = NULL;
	for (; i >= 0; --i) {
		if (find_next_variable(&loc, r, &v) == -1)
			return -1;
	}
	*name = strndup(v.name, v.namelen);
	if (*name == NULL)
		return 0;
	return 1;
}


int cfgtool_write_all(Cfgtool *r, FILE *f) {
	if (r->da.used > 0)
		if (fwrite(r->da.data, r->da.used, 1, f) != 1)
			return -1;
	return 0;
}


int cfgtool_read_all(Cfgtool *r, FILE *f, int set_existing) {
	Cfgtool *aux;
	char *name;
	void *value;
	size_t len;
	Var vv;
	int i, x;
	
	name = NULL;
	aux = cfgtool_init_repository();
	if (aux == NULL)
		return -1;
	
	if (file_read_open(f, &aux->da.data, &aux->da.used) == -1)
		goto error;
	aux->da.alloc = aux->da.used;

	for (i = 0; ; ++i) {
		x = cfgtool_ith_name(aux, i, &name);
		if (x == -1)
			break;
		if (x == 0)
			goto error;
		if (cfgtool_get(aux, name, &value, &len) == -1)
			goto error;
		if (set_existing || find_variable(name, r, &vv) == -1)
			if (put_variable(name, r, value, len) == -1)
				goto error;
		free(name);
	}

	cfgtool_close_repository(aux);
	return 0;

error:
	cfgtool_close_repository(aux);
	free(name);
	return -1;
}


/**********************************************************************/


/*
 * Function:	parse_variable
 * Purpose:	Parse variable at given location.
 */
static int parse_variable(byte *loc, Var *v) {
	static const char whitespace[] = " \t\n";

	loc += strspn(loc, whitespace);
	v->name = loc;
	v->namelen = strcspn(loc, whitespace);
	
	loc += v->namelen;
	loc += strspn(loc, whitespace);
	if (*loc != '=') {
		__publib_error("syntax error in value file: missing '='");
		return -1;
	}
	++loc;

	loc += strspn(loc, whitespace);
	v->value = loc;
	v->len = strcspn(loc, whitespace);
	
	loc += v->len;
	v->next = loc + strspn(loc, whitespace);

	return 0;
}


/*
 * Function:	find_next_variable
 * Purpose:	Find next variable in repository; loc == NULL means first.
 */
static int find_next_variable(byte **loc, Cfgtool *r, Var *v) {
	if (*loc == NULL)
		*loc = r->da.data;
	if (r->da.used == 0 || *loc == (byte *) r->da.data + r->da.used)
		return -1;
	if (parse_variable(*loc, v) == -1)
		return -1;
	*loc = v->next;
	return 0;
}


/*
 * Function:	find_variable
 * Purpose:	Find variable in repository.
 */
static int find_variable(const char *name, Cfgtool *r, Var *v) {
	byte *loc;
	
	loc = NULL;
	while (find_next_variable(&loc, r, v) == 0)
		if (strncmp(v->name, name, v->namelen) == 0 &&
		    name[v->namelen] == '\0')
			return 0;
	return -1;
}


/*
 * Function:	destroy_variable
 * Purpose:	Remove variable from repository-in-memory.
 */
static void destroy_variable(const char *name, Cfgtool *r) {
	Var v;

	if (find_variable(name, r, &v) == 0) {
		memmove(v.name, v.next,
			r->da.used - (v.next - (byte *) r->da.data));
		r->da.used -= v.next - v.name;
		r->dirty = 1;
	}
}


/*
 * Function:	decode_variable
 * Purpose:	Convert variable's value from base64 to octet stream.
 * Note:	Caller must free return value.
 */
static char *decode_variable(Var *v, size_t *n) {
	char *buf;
	
	buf = malloc(base64_length(v->len));
	if (buf == NULL) {
		__publib_error("out of memory");
		return NULL;
	}
	*n = base64_decode(buf, v->value, v->len);
	return buf;
}


/*
 * Function:	put_variable
 * Purpose:	Put variable into repository-in-memory.
 */
static int put_variable(const char *name, Cfgtool *r, const byte *value,
size_t len) {
	char *p;
	size_t n, nn, encoded_len;

	encoded_len = strlen(name) + 3 + base64_length(len) + 2;
	
	if (dynarr_resize(&r->da, r->da.used + encoded_len) == -1) {
		__publib_error("out of memory");
		return -1;
	}

	destroy_variable(name, r);
	
	p = (char *) r->da.data + r->da.used;
	n = strlen(name);
	memcpy(p, name, n);
	memcpy(p + n, " = ", 3);
	nn = base64_encode(p + n + 3, value, len);
	p[n + 3 + nn] = '\n';
	r->da.used += n + 3 + nn + 1;
	
	r->dirty = 1;
	
	return 0;
}
