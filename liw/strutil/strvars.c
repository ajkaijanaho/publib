/*
 * strvars.c -- expand variables in string
 *
 * Part of publib.  See man page for more information.
 * "@(#)publib-strutil:$Id: strvars.c,v 1.6 1995/08/24 16:51:36 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "publib/strutil.h"
#include "publib/errormsg.h"


/*
 * Add `s' to end of `*r', allocating more memory.
 */
static int add_to_end(char **r, size_t *size, const char *str, size_t n) {
	char *p;

	p = realloc(*r, *size + n);
	if (p == NULL) {
		__publib_error("realloc failed");
		return -1;
	}
	*r = p;
	memcpy(*r + *size, str, n);
	*size += n;
	return 0;
}


/*
 * Copy to next '$' or end of string (including '\0').  Return -1 for
 * error (out of memory), or 0 for '$' not found, or 1 for '$' found.
 * Advance *str appropriately.
 */
static int copy_to_dollar(char **r, size_t *size, const char **str) {
	size_t n;
	int ret;

	n = strcspn(*str, "$");
	assert((*str)[n] == '\0' || (*str)[n] == '$');
	if ((*str)[n] == '\0') {
		++n;
		ret = 0;
	} else
		ret = 1;

	if (n > 0) {
		if (add_to_end(r, size, *str, n) == -1)
			return -1;
		*str += n;
	}
	return ret;
}



/*
 * Get value of a variable.  Return NULL for failure, otherwise value.
 */
static char *value_of(const char **str, char *(*expand)(const char *)) {
	char *value;

	assert(str != NULL);
	assert(*str != NULL);
	assert(**str == '$');

	if ((*str)[1] == '\0') {
		value = "$";
		*str += 2;
	} else if ((*str)[1] == '(') {
		char *name;
		size_t n = strcspn(*str+2, ")");

		if ((*str)[n] == '\0') {
			__publib_error("syntax error for variable reference");
			return NULL;
		}
		*str += 2;
		name = strndup(*str, n);
		if (name == NULL) {
			__publib_error("strdup failed");
			return NULL;
		}
		value = expand(name);
		free(name);
		if (value == NULL) {
			__publib_error("unknown variable name");
			return NULL;
		}
		*str += n;
	} else {
		char onechar[] = "x";
		onechar[0] = (*str)[1];
		value = expand(onechar);
		if (value == NULL) {
			__publib_error("unknown variable name");
			return NULL;
		}
		*str += 2;
	}

	return value;
}



int strvars(char **res, const char *str, char *(*expand)(const char *)) {
	char *r, *value;
	size_t size;
	int ret;

	assert(res != NULL);
	assert(str != NULL);
	assert(expand != NULL);

	r = NULL;
	size = 0;

	for (;;) {
		ret = copy_to_dollar(&r, &size, &str);
		if (ret == -1)
			return -1;
		else if (ret == 0)
			break;

		value = value_of(&str, expand);
		if (value == NULL)
			return -1;

		if (add_to_end(&r, &size, value, strlen(value) + 1) == -1)
			return -1;
	}

	*res = r;
	return 0;
}
