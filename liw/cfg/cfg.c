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
 */
#define MAXERR 128
static char error[80 + MAXERR] = "";



/*
 * Hold the current (while cfg_read_file is running) filename and line
 * number.
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
 * Prototypes for local functions.
 */
static void mkerror(const char *, ...);
static int parse_line(char *, char **, char **);
static struct cfg_variable *lookup_name(struct cfg_variable *, const char *);
static struct cfg_variable *lookup_prefix(struct cfg_variable *, const char *);
static struct cfg_variable *lookup_option(struct cfg_variable *, int);
static struct type *lookup_type(enum cfg_type);
static int write_variable(FILE *, struct cfg_variable *);
static FILE *open_file(const char *, const char *);

static int single_char_options(struct cfg_variable *, char **, int *);
static int long_option(struct cfg_variable *, char **, int *);

static int str_to_long(const char *, long *);
static int str_to_ulong(const char *, unsigned long *);
static int str_to_double(const char *, double *);

static int set_long(void *, char **);
static int set_long_expr(void *, char **);
static int set_ulong(void *, char **);
static int set_ulong_expr(void *, char **);
static int set_double(void *, char **);
static int set_double_expr(void *, char **);
static int set_string(void *, char **);
static int set_boolean(void *, char **);

static int write_long(FILE *f, void *);
static int write_ulong(FILE *f, void *);
static int write_double(FILE *f, void *);
static int write_string(FILE *f, void *);
static int write_boolean(FILE *f, void *);



/*
 * Describe a type.  The two function pointers here will point to functions
 * that parse a value and set it to the value of a variable, and write
 * out a value.
 */
struct type {
	enum cfg_type type;
	int (*set_value)(void *, char **);
	int (*write)(FILE *, void *);
};


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



/*
 * Read configuration file values from a file.  Return -1 for error,
 * 0 for OK.
 */
int cfg_read_file(struct cfg_variable *variables, const char *fn) {
	FILE *f;
	int ret;
	char *line, *value, *varname;
	struct cfg_variable *v;

	assert(variables != NULL);
	assert(fn != NULL);
	assert(*fn != '\0');

	filename = fn;
	lineno = 0;

	f = fopen(fn, "r");
	if (f == NULL) {
		mkerror("couldn't open file for reading: %.*s", 
			MAXERR, strerror(errno));
		return -1;
	}

	ret = 0;
	lineno = 0;
	while (ret == 0 && (line = getaline(f)) != NULL) {
		++lineno;
		switch (parse_line(line, &varname, &value)) {
		case 1: break;
		case 0:
			free(line);
			continue;
		case -1:
			free(line);
			ret = -1;
			continue;
		default:		/* unknown return from parse_line */
			assert(0);
			abort();
		}

		v = lookup_name(variables, varname);
		if (v == NULL) {
			mkerror("unknown variable %.*s", MAXERR, varname);
			ret = -1;
		} else if (!(v->cfg_flags & cfg_set_from_array)) {
			struct type *t = lookup_type(v->cfg_type);
			assert(t != NULL);
			if (t->set_value(v->cfg_value, &value) == -1)
				ret = -1;
		}
		if (value != NULL)
			free(line);	/* yes, line, not value */
	}

	if (ferror(f)) {
		mkerror("unknown error occured while reading");
		ret = -1;
	}
	if (fclose(f) == EOF) {
		mkerror("unknown error occured while closing");
		ret = -1;
	}
	assert(ret != -1 || *error != '\0');
	return ret;
}



/*
 * Write configuration data into file.
 */
int cfg_write_file(struct cfg_variable *variables, const char *fn) {
	struct cfg_variable *p;
	struct type *t;
	int i, ret;
	FILE *f;

	filename = fn;
	lineno = 0;

	f = fopen(fn, "w");
	if (f == NULL) {
		mkerror("error opening file for writing: %.*s", 
			MAXERR, strerror(errno));
		return -1;
	}

	ret = 0;
	for (i = 0; p = variables + i, p->cfg_name != NULL; ++i) {
		if (p->cfg_flags & cfg_read_only)
			continue;

		fprintf(f, "%s = ", p->cfg_name);

		t = lookup_type(p->cfg_type);
		assert(t != NULL);
		if (t->write(f, p->cfg_value) == -1)
			ret = -1;

		fprintf(f, "\n");
		++lineno;
		if (ferror(f)) {
			mkerror("error writing to file");
			ret = -1;
			break;
		}
	}

	if (fclose(f) == EOF) {
		mkerror("unknown error occured while closing");
		ret = -1;
	}
	return ret;
}



/*
 * Write configuration data into file, keep existing contents as much as
 * possible.
 */
int cfg_rewrite_file(struct cfg_variable *variables, const char *fn) {
	struct cfg_variable *v;
	int ret;
	FILE *old, *new;
	char buf[10240];
	char bak[10240];
	char *line, *orig, *value, *varname;

	filename = buf;
	lineno = 0;
	
	sprintf(buf, "%.10000s.new", fn);
	sprintf(bak, "%.10000s.bak", fn);

	old = new = NULL;
	orig = line = NULL;

	old = open_file(fn, "r");
	new = open_file(buf, "w");
	if (old == NULL || new == NULL)
		goto error;

	for (v = variables; v->cfg_name != NULL; ++v)
		v->cfg_flags &= ~cfg_saved;

	ret = 0;
	while (ret == 0 && (line = getaline(old)) != NULL) {
		orig = strdup(line);
		if (orig == NULL) {
			mkerror("strdup failed, out of memory?");
			goto error;
		}

		++lineno;
		if (parse_line(line, &varname, &value) <= 0 ||
		    (v = lookup_name(variables, varname)) == NULL) {
			fprintf(new, "%s\n", orig);
		} else if (write_variable(new, v) == -1) {
			mkerror("error writing to %.*s", MAXERR, buf);
			goto error;
		} else
			v->cfg_flags |= cfg_saved;
		
		++lineno;
		free(line);
		free(orig);
	}

	for (v = variables; v->cfg_name != NULL; ++v) {
		if ((v->cfg_flags & cfg_saved) != cfg_saved) {
			if (write_variable(new, v) == -1) {
				mkerror("error writing to %.*s", MAXERR, buf);
				goto error;
			}
			++lineno;
		}
	}
			
	if (ferror(new)) {
		mkerror("error writing to %.*s", MAXERR, buf);
		goto error;
	}
	
	if (fclose(new) == EOF) {
		mkerror("unknown error occured while closing %.*s", 
			MAXERR, buf);
		return -1;
	}
	
	if (fclose(old) == EOF) {
		mkerror("unknown error occured while closing %.*s", 
			MAXERR, fn);
		return -1;
	}
	
	if (rename(fn, bak) == EOF) {
		mkerror("error renaming %.*s to %.*s", 
			MAXERR/2, fn, MAXERR/2, bak);
		return -1;
	}
	
	if (rename(buf, fn) == EOF) {
		mkerror("error renaming %.*s to %.*s", 
			MAXERR/2, buf, MAXERR/2, fn);
		return -1;
	}
	
	return ret;
	
error:
	free(orig);
	free(line);
	if (old != NULL) (void) fclose(old);
	if (new != NULL) (void) fclose(new);
	(void) remove(buf);
	return -1;
}



/*******************************************************************
 * Local functions only below this.
 */



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
 * Find the descriptor for a variable based on its name.  Return NULL if
 * not found.
 */
static struct cfg_variable *lookup_name(struct cfg_variable *vars,
const char *s) {
	while (vars->cfg_name != NULL && strcmp(vars->cfg_name, s) != 0)
		++vars;
	return vars->cfg_name == NULL ? NULL : vars;
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
	if (t == NULL)
		return -1;
	fprintf(f, "%s = ", v->cfg_name);
	if (t->write(f, v->cfg_value) == -1)
		return -1;
	fprintf(f, "\n");
	return 0;
}


static FILE *open_file(const char *name, const char *mode) {
	FILE *f;
	
	f = fopen(name, mode);
	if (f == NULL) {
		mkerror("error opening %.*s for %.*s: %.*s",
			MAXERR/3, name,
			MAXERR/3, (*mode == 'r') ? "reading" : "writing",
			MAXERR/3, strerror(errno));
		return NULL;
	}
	return f;
}


/**********************************************************************
 * Functions to set and write out values of various types.  All of these
 * functions are very similar, so they're not commented on separately.
 */

static int set_long(void *var, char **value) {
	return str_to_long(*value, var);
}

static int write_long(FILE *f, void *var) {
	fprintf(f, "%ld", *(long *) var);
	return ferror(f) ? -1 : 0;
}

static int set_ulong(void *var, char **value) {
	return str_to_ulong(*value, var);
}

static int write_ulong(FILE *f, void *var) {
	fprintf(f, "%lu", *(unsigned long *) var);
	return ferror(f) ? -1 : 0;
}

static int set_double(void *var, char **value) {
	return str_to_double(*value, var);
}

static int write_double(FILE *f, void *var) {
	fprintf(f, "%f", *(double *) var);
	return ferror(f) ? -1 : 0;
}

static int set_string(void *var, char **value) {
	char *p, *q, *mem;
	size_t max;

	assert(value != NULL);
	assert(*value != NULL);

	for (p = *value; isspace(*p); ++p)
		break;

	if (*p != '"') {
#if 0
		mem = *value;
		*value = NULL;
#else
		mem = strdup(*value);
		if (mem == NULL) {
			mkerror("out of memory");
			return -1;
		}
#endif
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

static int set_boolean(void *var, char **value) {
	int i;

	for (i = 0; i < num_boolnames; ++i) {
		if (strcasecmp(*value, boolnames[i]) == 0) {
			*(int *) var = (i % 2);
			return 0;
		}
	}
	mkerror("invalid Boolean value: `%.*s'", MAXERR, *value);
	return -1;
}

static int write_boolean(FILE *f, void *var) {
	fprintf(f, "%s", boolnames[*(int *) var]);
	return ferror(f) ? -1 : 0;
}


static int no_nothing(const char *sym, int nargs, double *args, double *val) {
	return -1;
}
static int set_long_expr(void *var, char **value) {
	double result;
	if (expr_eval(*value, &result, no_nothing) == -1) {
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
static int set_ulong_expr(void *var, char **value) {
	double result;
	if (expr_eval(*value, &result, no_nothing) == -1) {
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
static int set_double_expr(void *var, char **value) {
	int ret;

	ret = expr_eval(*value, var, no_nothing);
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
	ret = t->set_value(v->cfg_value, &s);
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
