/*
 * cfg.c -- implement simple configuration files
 *
 * Lars Wirzenius
 */
 
#include "publib/cfg-config.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "publib/cfg.h"
#include "publib/expr.h"
#include "publib/files.h"
#include "publib/strutil.h"


/*
 * Use strcmp if strcasecmp is not available.  Ditto for strncasecmp/strncmp.
 */
#if !HAVE_STRCASECMP
#define strcasecmp(a,b) strcmp(a,b)
#endif
#if !HAVE_STRNCASECMP
#define strncasecmp(a,b,n) strncmp(a,b,n)
#endif


/*
 * Buffer for the last error message.
 * MAXERR is maximum number of chars from variable string arguments.
 *
 * XXX this is a bug, since it's non-re-entrant. Should be fixed one day.
 */
#define MAXERR 128
static char error[80 + MAXERR] = "";



/*
 * Hold the current (while cfg_read_file is running) filename and line
 * number.
 *
 * XXX these are also a bug, since they're also non-re-entrant.
 */
static const char *filename;
static long lineno;


/*
 * Possible names for boolean values.  Even indexes must be false,
 * odd indexes must be true.
 */
static const char * const boolnames[] = {
	"no", "yes",
	"false", "true",
};
static const int num_boolnames = sizeof(boolnames) / sizeof(*boolnames);



/*
 * Describe a type.  The two function pointers here will point to functions
 * that parse a value and set it to the value of a variable, and write
 * out a value.
 */
struct type {
	enum cfg_type type;
	int (*set_value)(void *, char *);
	int (*write)(FILE *, void *);
};


/*
 * Prototypes for local functions.
 */
static void mkerror(const char *, ...);
static int parse_line(char *, char **, char **);
static struct cfg_variable *lookup_prefix(struct cfg_variable *, const char *);
static struct cfg_variable *lookup_option(struct cfg_variable *, int);
static struct type *lookup_type(enum cfg_type);
static int write_variable(FILE *, struct cfg_variable *);
static int write_unknown(FILE *, char *, char *);

static int unknowns_allowed(struct cfg_variable *);
static int process_file(FILE *, FILE *, int (*)(FILE *, char *),
	int (*)(FILE *, char *, char *, struct cfg_variable *), 
	struct cfg_variable *);
static int do_nonvar_ignore(FILE *, char *);
static int do_nonvar_write(FILE *, char *);
static int do_var_set(FILE *, char *, char *, struct cfg_variable *);
static int set_value(struct cfg_variable *, char *);
static int set_unknown(struct cfg_unknown_variable *, char *);
static int add_unknown(struct cfg_unknown_variable **, char *, char *);
static int do_var_write(FILE *, char *, char *, struct cfg_variable *);
static struct cfg_variable *find_known(struct cfg_variable *, char *);
static int process_without_flag(struct cfg_variable *, unsigned, void *,
	int (*)(void *, struct cfg_variable *),
	int (*)(void *, struct cfg_unknown_variable *));
static int do_known_write(void *, struct cfg_variable *);
static int do_unknown_write(void *, struct cfg_unknown_variable *);

static int open_file(FILE **, const char *, const char *);
static int close_file(FILE *, const char *);
static int rename_file(const char *, const char *);

static int single_char_options(struct cfg_variable *, char **, int *);
static int long_option(struct cfg_variable *, char **, int *);

static int str_to_long(const char *, long *);
static int str_to_ulong(const char *, unsigned long *);
static int str_to_double(const char *, double *);

static int set_long(void *, char *);
static int set_long_expr(void *, char *);
static int set_ulong(void *, char *);
static int set_ulong_expr(void *, char *);
static int set_double(void *, char *);
static int set_double_expr(void *, char *);
static int set_string(void *, char *);
static int set_boolean(void *, char *);

static int write_long(FILE *f, void *);
static int write_ulong(FILE *f, void *);
static int write_double(FILE *f, void *);
static int write_string(FILE *f, void *);
static int write_boolean(FILE *f, void *);



/*
 * List of types and their conversion functions.
 */
static struct type types[] = {
	{ CFG_LONG,		set_long,	write_long },
	{ CFG_LONG_EXPR,	set_long_expr,	write_long },
	{ CFG_ULONG,		set_ulong,	write_ulong },
	{ CFG_ULONG_EXPR,	set_ulong_expr,	write_ulong },
	{ CFG_DOUBLE,		set_double,	write_double },
	{ CFG_DOUBLE_EXPR,	set_double_expr, write_double },
	{ CFG_STRING, 		set_string,	write_string },
	{ CFG_BOOLEAN,		set_boolean,	write_boolean },
	{ 0,			NULL,		NULL },
};



/*
 * Return an error message explaining the last error.
 */
const char *cfg_error(void) {
	return error;
}



/*
 * Read an argv-style array for configuration values.  Return -1 for error,
 * 0 for OK.
 *
 * This function replaces getopt.  The argv argument is like main's argv, 
 * but it starts with the first potential option, not with the program
 * name.  This function removes options from the array (by moving later
 * elements downwards); options are like for the GNU getopt_long funtion.
 * The configuration file variable name is the long option name.
 */
int cfg_from_array(struct cfg_variable *variables, char **argv) {
	int i, j;
	
	for (i = 0; argv[i] != NULL && argv[i][0] == '-'; ++i) {
		if (argv[i][1] == '-') {
			if (long_option(variables, argv, &i) == -1)
				return -1;
		} else if (single_char_options(variables, argv, &i) == -1)
			return -1;
	}

	if (i > 0) {
		j = 0;
		do {
			argv[j] = argv[i];
			++j;
		} while (argv[i++] != NULL);
	}
	return 0;
}


int cfg_read_open_file(struct cfg_variable *variables, const char *fn, FILE *f)
{
	assert(variables != NULL);
	assert(fn != NULL);
	assert(*fn != '\0');
	assert(f != NULL);

	filename = fn;
	return process_file(f, NULL, do_nonvar_ignore, do_var_set, variables);
}


/*
 * Read configuration file values from a file.  Return -1 for error,
 * 0 for OK.
 */
int cfg_read_file(struct cfg_variable *variables, const char *fn) {
	FILE *f;
	int ret;

	assert(variables != NULL);
	assert(fn != NULL);
	assert(*fn != '\0');

	filename = fn;
	if (open_file(&f, fn, "r") == -1)
		return -1;
	ret = process_file(f, NULL, do_nonvar_ignore, do_var_set, variables);
	if (close_file(f, fn) == -1)
		ret = -1;
	return ret;
}


int cfg_write_open_file(struct cfg_variable *vars, const char *fn, FILE *f) {
	assert(vars != NULL);
	assert(fn != NULL);
	assert(*fn != '\0');
	assert(f != NULL);

	filename = fn;
	cfg_reset_some_flags(vars, cfg_saved);
	return process_without_flag(vars, cfg_saved, f, do_known_write, 
				 do_unknown_write);
}


/*
 * Write configuration data into file.
 */
int cfg_write_file(struct cfg_variable *vars, const char *fn) {
	FILE *f;
	int ret;

	assert(vars != NULL);
	assert(fn != NULL);
	assert(*fn != '\0');

	filename = fn;
	if (open_file(&f, fn, "w") == -1)
		return -1;
	cfg_reset_some_flags(vars, cfg_saved);
	ret = process_without_flag(vars, cfg_saved, f, do_known_write, 
				 do_unknown_write);
	if (close_file(f, fn) == -1)
		ret = -1;
	return ret;
}



/*
 * Write configuration data into file, keep existing contents as much as
 * possible.
 */
int cfg_rewrite_file(struct cfg_variable *variables, const char *fn) {
	FILE *old, *new;
	char buf[10240];
	char bak[10240];
	int ret;

	assert(variables != NULL);
	assert(fn != NULL);
	assert(*fn != '\0');

	filename = buf;
	sprintf(buf, "%.10000s.new", fn);
	sprintf(bak, "%.10000s.bak", fn);

	old = new = NULL;

	if (open_file(&old, fn, "r") == -1 || open_file(&new, buf, "w") == -1)
		goto error;

	cfg_reset_some_flags(variables, cfg_saved);
	ret = process_file(old, new, do_nonvar_write, do_var_write, variables);
	if (process_without_flag(variables, cfg_saved, new, do_known_write, 
				 do_unknown_write) == -1)
		ret = -1;

	if (close_file(old, fn) == -1 || close_file(new, buf) == -1 ||
	    rename_file(fn, bak) == -1)
		goto error;
	/* XXX at this point, the old file doesn't exist anymore; use link
	   instead of rename so that no-one will be confused by the temporary
	   disappearance. */
	if (rename_file(buf, fn) == EOF)
		return -1;
	if (ret == 0)
		(void) remove(bak);
	return ret;
	
error:
	if (old != NULL) (void) fclose(old);
	if (new != NULL) (void) fclose(new);
	(void) remove(buf);
	return -1;
}


struct cfg_unknown_variable *cfg_find_unknown(struct cfg_variable *vars,
const char *name) {
	struct cfg_unknown_variable *u, **ptr_to_head;
	
	assert(unknowns_allowed(vars));
	ptr_to_head = vars->cfg_value;
	for (u = *ptr_to_head; u != NULL; u = u->cfg_next)
		if (strcmp(name, u->cfg_name) == 0)
			return u;
	return NULL;
}


void cfg_reset_some_flags(struct cfg_variable *vars, unsigned which) {
	struct cfg_variable *v;
	struct cfg_unknown_variable *u, **ptr_to_head;
	
	for (v = vars; v->cfg_name != NULL; ++v)
		v->cfg_flags &= ~which;
	if (unknowns_allowed(vars)) {
		ptr_to_head = vars->cfg_value;
		for (u = *ptr_to_head; u != NULL; u = u->cfg_next)
			u->cfg_flags &= ~which;
	}
}


/*******************************************************************
 * Local functions only below this.
 */


static int unknowns_allowed(struct cfg_variable *vars) {
	if (vars->cfg_name == NULL || vars->cfg_name[0] == '\0')
		return vars->cfg_value != NULL;
	return 0;
}

static int process_file(FILE *in, FILE *out, int (*do_nonvar)(FILE *, char *),
int (*do_var)(FILE *, char *, char *, struct cfg_variable *), 
struct cfg_variable *vars) {
	char *line, *varname, *value;
	int ret;

	lineno = 0;
	ret = 0;
	while ((line = getaline(in)) != NULL) {
		switch (parse_line(line, &varname, &value)) {
		case -1:
			ret = -1;
			break;
		case 0:
			if (do_nonvar(out, line) == -1)
				ret = -1;
			break;
		case 1:
			if (do_var(out, varname, value, vars) == -1)
				ret = -1;
			break;
		}
		free(line);
	}
	return ret;
}


static int do_nonvar_ignore(FILE *out, char *line) {
	return 0;
}

static int do_nonvar_write(FILE *out, char *line) {
	fprintf(out, "%s\n", line);
	return 0;
}

static int do_var_set(FILE *out, char *varname, char *value,
struct cfg_variable *vars) {
	struct cfg_variable *v;
	struct cfg_unknown_variable *u;

	v = find_known(vars, varname);
	if (v != NULL)
		return set_value(v, value);
	if (!unknowns_allowed(vars)) {
		mkerror("unknown variable `%.*s'", MAXERR, varname);
		return -1;
	}
	u = cfg_find_unknown(vars, varname);
	if (u != NULL)
		return set_unknown(u, value);
	return add_unknown(vars->cfg_value, varname, value);
}

static int set_value(struct cfg_variable *v, char *value) {
	struct type *t;
	
	t = lookup_type(v->cfg_type);
	assert(t != NULL);
	return t->set_value(v->cfg_value, value);
}

static int set_unknown(struct cfg_unknown_variable *u, char *value) {
	/* XXX can t be made static? */
	struct type *t;
	
	free(u->cfg_value);
	t = lookup_type(CFG_STRING);
	assert(t != NULL);
	return t->set_value(&u->cfg_value, value);
}

static int add_unknown(struct cfg_unknown_variable **ptr_to_head,
char *varname, char *value) {
	struct cfg_unknown_variable *u;
	
	u = malloc(sizeof(*u));
	varname = strdup(varname);
	if (u == NULL || varname == NULL) {
		mkerror("malloc failed, out of memory?");
		goto error;
	}
	u->cfg_name = varname;
	u->cfg_value = NULL;
	u->cfg_flags = 0;
	if (set_unknown(u, value) == -1)
		goto error;

	u->cfg_next = *ptr_to_head;
	*ptr_to_head = u;
	return 0;

error:
	free(u);
	free(varname);
	return -1;
}

static int do_var_write(FILE *out, char *varname, char *value,
struct cfg_variable *vars) {
	struct cfg_variable *v;
	struct cfg_unknown_variable *u;
	
	v = find_known(vars, varname);
	if (v != NULL) {
		v->cfg_flags |= cfg_saved;
		if ((v->cfg_flags & cfg_removed) == cfg_removed)
			return 0;
		return write_variable(out, v);
	}
	if (!unknowns_allowed(vars)) {
		mkerror("unknown variable `%.*s' removed", MAXERR, varname);
		return -1;
	}
	u = cfg_find_unknown(vars, varname);
	if (u != NULL) {
		u->cfg_flags |= cfg_saved;
		if ((u->cfg_flags & cfg_removed) == cfg_removed)
			return 0;
		return write_unknown(out, varname, u->cfg_value);
	}
	return write_unknown(out, varname, value);
}


static struct cfg_variable *find_known(struct cfg_variable *vars, char *name) {
	struct cfg_variable *v;
	
	for (v = vars; v->cfg_name != NULL; ++v) {
		if (strcmp(v->cfg_name, name) == 0)
			return v;
	}
	return NULL;
}


static int process_without_flag(struct cfg_variable *vars, unsigned which,
void *arg, int (*do_known)(void *, struct cfg_variable *),
int (*do_unknown)(void *, struct cfg_unknown_variable *)) {
	struct cfg_variable *v;
	struct cfg_unknown_variable *u, **ptr_to_head;
	
	for (v = vars; v->cfg_name != NULL; ++v)
		if (v->cfg_name[0] != '\0' && (v->cfg_flags & which) == 0)
			if (do_known(arg, v) == -1)
				return -1;
	if (unknowns_allowed(vars)) {
		ptr_to_head = vars->cfg_value;
		for (u = *ptr_to_head; u != NULL; u = u->cfg_next) {
			if ((u->cfg_flags & which) == 0) {
				if (do_unknown(arg, u) == -1)
					return -1;
			}
		}
	}
	return 0;
}

static int do_known_write(void *arg, struct cfg_variable *v) {
	if ((v->cfg_flags & cfg_removed) == cfg_removed)
		return 0;
	return write_variable(arg, v);
}

static int do_unknown_write(void *arg, struct cfg_unknown_variable *u) {
	if ((u->cfg_flags & cfg_removed) == cfg_removed)
		return 0;
	return write_unknown(arg, u->cfg_name, u->cfg_value);
}


static int open_file(FILE **fp, const char *fn, const char *mode) {
	*fp = fopen(fn, mode);
	if (*fp == NULL) {
		mkerror("couldn't open file `%.*s': %.*s", MAXERR/2, fn,
			MAXERR/2, strerror(errno));
		return -1;
	}
	return 0;
}

static int close_file(FILE *f, const char *fn) {
	int ret;
	
	ret = 0;
	if (ferror(f)) {
		mkerror("error occured when processing file `%.*s'",
			MAXERR, fn);
		ret = -1;
	}
	if (fclose(f) == EOF) {
		mkerror("error occured when closing file `%.*s'",
			MAXERR, fn);
		ret = -1;
	}
	return ret;
}

static int rename_file(const char *oldname, const char *newname) {
	if (rename(oldname, newname) == EOF) {
		mkerror("error renaming `%.*s' to `%.*s'", 
			MAXERR/2, oldname, MAXERR/2, newname);
		return -1;
	}
	return 0;
}


/*
 * Parse a line.  Return -1 for syntax error, 0 for no variable on line,
 * 1 for variable on line.  Return name of variable via *varname, and
 * a pointer to the value (undigested) via *value.
 *
 * Note that the original line will be modified.
 */
static int parse_line(char *line, char **varname, char **value) {
	char *p;

	for (; isspace(*line); ++line)
		continue;
	if (*line == '#' || *line == '\0')
		return 0;

	for (p = line; isgraph(*p) && *p != '='; ++p)
		continue;

	while (isspace(*p))
		++p;

	if (*p != '=') {
		mkerror("no assignment on this line");
		return -1;
	}

	*p = '\0';
	*varname = line;
	*value = p+1;

	for (; p > line && isspace(p[-1]); --p)
		continue;
	*p = '\0';

	while (isspace(**value))
		++(*value);
	for (p = strchr(*value, '\0'); p > *value && isspace(p[-1]); --p)
		continue;
	*p = '\0';

	return 1;
}



/*
 * Find the descriptor for a variable based on its name.  Name may be
 * a unique prefix.  Return NULL if not found.
 */
static struct cfg_variable *lookup_prefix(struct cfg_variable *vars,
const char *s) {
	struct cfg_variable *v;
	size_t n;
	
	v = NULL;
	n = strlen(s);
	while (vars->cfg_name != NULL && strcasecmp(vars->cfg_name, s) != 0) {
		if (strncasecmp(vars->cfg_name, s, n) == 0) {
			if (v != NULL)
				return NULL;	/* can't be unique prefix */
			v = vars;
		}
		++vars;
	}
	return v;
}



/*
 * Find the descriptor for a variable based on its short option.  
 * Return NULL if not found.
 */
static struct cfg_variable *lookup_option(struct cfg_variable *vars, int opt) {
	while (vars->cfg_name != NULL && vars->cfg_option != opt)
		++vars;
	return vars->cfg_name == NULL ? NULL : vars;
}



/*
 * Find the type descriptor for type t.
 */
static struct type *lookup_type(enum cfg_type t) {
	int i;

	for (i = 0; types[i].write != NULL; ++i)
		if (types[i].type == t)
			return types + i;
	return NULL;
}


/*
 * Convert a value to a signed long, checking for errors.
 * Return -1 for error, 0 for ok, and the converted value via *value.
 */
static int str_to_long(const char *str, long *value) {
	char *e;

	errno = 0;
	*value = strtol(str, &e, 10);

	if (*e != '\0' || e == str) {
		mkerror("not an signed integer value: `%.*s'", MAXERR, str);
		return -1;
	}

	if ((*value == LONG_MIN || *value == LONG_MAX) && errno == ERANGE) {
		mkerror("value too large or small: `%.*s'", MAXERR, str);
		return -1;
	}

	return 0;
}



/*
 * Convert a value to an unsigned long, checking for errors.
 * Return -1 for error, 0 for ok, and the converted value via *value.
 */
static int str_to_ulong(const char *str, unsigned long *value) {
	char *e;

	errno = 0;

	while (isspace(*str))
		++str;
	if (*str == '-') {
		mkerror("negative values not allowed: `%.*s'", MAXERR, str);
		return -1;
	}

	*value = strtoul(str, &e, 10);

	if (*e != '\0' || e == str) {
		mkerror("not an unsigned integer value: `%.*s'", MAXERR, str);
		return -1;
	}

	if (*value == ULONG_MAX && errno == ERANGE) {
		mkerror("value too large: `%.*s'", MAXERR, str);
		return -1;
	}

	return 0;
}



/*
 * Convert a value to a double, checking for errors.
 * Return -1 for error, 0 for ok, and the converted value via *value.
 */
static int str_to_double(const char *str, double *value) {
	char *e;

	errno = 0;
	*value = strtod(str, &e);

	if (*e != '\0' || e == str) {
		mkerror("not a floating point value: `%.*s'", MAXERR, str);
		return -1;
	}

	if ((*value == -HUGE_VAL || *value == HUGE_VAL) && errno == ERANGE) {
		mkerror("value too large or small: `%.*s'", MAXERR, str);
		return -1;
	}

	return 0;
}



/*
 * Format an error message.
 */
static void mkerror(const char *fmt, ...) {
	va_list args;
	char *p;

	if (lineno == 0)
		sprintf(error, "%s: ", filename);
	else
		sprintf(error, "%s:%ld: ", filename, lineno);
	p = strchr(error, '\0');

	va_start(args, fmt);
	vsprintf(p, fmt, args);
	assert(strlen(error) < sizeof(error));
	va_end(args);
}


static int write_variable(FILE *f, struct cfg_variable *v) {
	struct type *t;

	t = lookup_type(v->cfg_type);
	assert(t != NULL);
	fprintf(f, "%s = ", v->cfg_name);
	if (t->write(f, v->cfg_value) == -1)
		return -1;
	fprintf(f, "\n");
	return 0;
}


static int write_unknown(FILE *f, char *name, char *value) {
	/* XXX can t be made static? */
	struct type *t;

	t = lookup_type(CFG_STRING);
	assert(t != NULL);
	fprintf(f, "%s = ", name);
	if (t->write(f, &value) == -1)
		return -1;
	fprintf(f, "\n");
	return 0;
}


/**********************************************************************
 * Functions to set and write out values of various types.  All of these
 * functions are very similar, so they're not commented on separately.
 */

static int set_long(void *var, char *value) {
	return str_to_long(value, var);
}

static int write_long(FILE *f, void *var) {
	fprintf(f, "%ld", *(long *) var);
	return ferror(f) ? -1 : 0;
}

static int set_ulong(void *var, char *value) {
	return str_to_ulong(value, var);
}

static int write_ulong(FILE *f, void *var) {
	fprintf(f, "%lu", *(unsigned long *) var);
	return ferror(f) ? -1 : 0;
}

static int set_double(void *var, char *value) {
	return str_to_double(value, var);
}

static int write_double(FILE *f, void *var) {
	fprintf(f, "%f", *(double *) var);
	return ferror(f) ? -1 : 0;
}

static int set_string(void *var, char *value) {
	char *p, *q, *mem;
	size_t max;

	assert(value != NULL);
	assert(*value != NULL);

	for (p = value; isspace(*p); ++p)
		break;

	if (*p != '"') {
		mem = strdup(value);
		if (mem == NULL) {
			mkerror("out of memory");
			return -1;
		}
	} else {
		strrtrim(p);
		q = strend(p);
		if (q == p || q[-1] != '"') {
			mkerror("invalid string syntax: `%.*s'", MAXERR, p);
			return -1;
		}

		++p;
		q[-1] = '\0';

		max = strlen(p) + 1;
		mem = malloc(max);
		if (mem == NULL) {
			mkerror("out of memory");
			return -1;
		}
		struncstr(mem, p, max-1);
		mem[max-1] = '\0';
	}

	*(char **) var = mem;
	return 0;
}

static int write_string(FILE *f, void *var) {
	char *str, *str2;
	size_t len, max;

	str = *(char **) var;

	if (str == NULL)
		fprintf(f, "\"\"");
	else {
		len = strlen(str);
		max = len * (CHAR_BIT/4+3) + 1;
		str2 = malloc(max);
		if (str2 == NULL) {
			mkerror("out of memory");
			return -1;
		}
		strcstr(str2, max, str, len);
		fprintf(f, "\"%s\"", str2);
		free(str2);
	}

	return ferror(f) ? -1 : 0;
}

static int set_boolean(void *var, char *value) {
	int i;

	for (i = 0; i < num_boolnames; ++i) {
		if (strcasecmp(value, boolnames[i]) == 0) {
			*(int *) var = (i % 2);
			return 0;
		}
	}
	mkerror("invalid Boolean value: `%.*s'", MAXERR, value);
	return -1;
}

static int write_boolean(FILE *f, void *var) {
	fprintf(f, "%s", boolnames[*(int *) var]);
	return ferror(f) ? -1 : 0;
}


static int no_nothing(const char *sym, int nargs, double *args, double *val) {
	return -1;
}
static int set_long_expr(void *var, char *value) {
	double result;
	if (expr_eval(value, &result, no_nothing) == -1) {
		mkerror("invalid expression");
		return -1;
	}
	if (result < LONG_MIN || result > LONG_MAX) {
		mkerror("result is out of range");
		return -1;
	}
	*(long *) var = (long) result;
	return 0;
}
static int set_ulong_expr(void *var, char *value) {
	double result;
	if (expr_eval(value, &result, no_nothing) == -1) {
		mkerror("invalid expression");
		return -1;
	}
	if (result < 0 || result > ULONG_MAX) {
		mkerror("result is out of range");
		return -1;
	}
	*(unsigned long *) var = (unsigned long) result;
	return 0;
}
static int set_double_expr(void *var, char *value) {
	int ret;

	ret = expr_eval(value, var, no_nothing);
	if (ret == -1)
		mkerror("invalid expression");
	return ret;
}


/*************/



static int set_value_from_option(struct cfg_variable *v, char **argv, int *i, 
char **rest) {
	struct type *t;
	char *s;
	int ret;
	
	if (v->cfg_type == CFG_BOOLEAN)
		s = strdup(boolnames[1]);
	else if (**rest == '\0') {
		if (argv[(*i) + 1] == NULL) {
			if (v->cfg_option != '\0')
				mkerror("option %c (--%.*s) lacks argument",
					v->cfg_option, MAXERR, v->cfg_name);
			else
				mkerror("option --%.*s lacks argument",
					MAXERR, v->cfg_name);
			return -1;
		}
		++(*i);
		s = strdup(argv[*i]);
	} else {
		if (**rest == '=')
			++(*rest);
		s = strdup(*rest);
		*rest = "x";
	}
	
	if (s == NULL) {
		mkerror("strdup failed, out of memory?");
		return -1;
	}
	
	t = lookup_type(v->cfg_type);
	assert(t == NULL);
	ret = t->set_value(v->cfg_value, s);
	if (ret != -1)
		v->cfg_flags |= cfg_set_from_array;
	free(s);
	return ret;
}



static int single_char_options(struct cfg_variable *vars, char **argv, int *i){
	struct cfg_variable *v;
	char *p;
	
	assert(vars != NULL);
	assert(argv != NULL);
	assert(i != NULL);
	assert(argv[*i] != NULL);
	assert(argv[*i][0] == '-');

	for (p = argv[*i] + 1; *p != '\0'; ++p) {
		v = lookup_option(vars, *p);
		if (v == NULL) {
			mkerror("unknown option character %c", *p);
			return -1;
		}
		if (v->cfg_type != CFG_BOOLEAN)
			++p;
		if (set_value_from_option(v, argv, i, &p) == -1)
			return -1;
	}
	return 0;
}



static int long_option(struct cfg_variable *vars, char **argv, int *i) {
	struct cfg_variable *v;
	char *p, buf[1024];
	
	assert(vars != NULL);
	assert(argv != NULL);
	assert(i != NULL);
	assert(argv[*i] != NULL);
	assert(strspn(argv[*i], "-") >= 2);

	p = argv[*i] + 2;
	strmaxcpy(buf, p, sizeof(buf));
	buf[strcspn(buf, "=")] = '\0';

	v = lookup_prefix(vars, buf);
	if (v == NULL) {
		mkerror("unknown option %.*s", MAXERR, buf);
		return -1;
	}

	p += strcspn(p, "=");
	return set_value_from_option(v, argv, i, &p);
}
