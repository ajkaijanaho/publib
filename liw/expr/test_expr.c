/*
 * test_expr.c -- test the expression parser and evaluator
 *
 * This program reads expressions from the standard input, one per line, and
 * prints the result to the standard output.  It defines the variables "pi",
 * "e", and "da", and the functions "sin", "cos", "ln", and "sqrt".
 *
 * This program is part of publib.
 * "@(#)publib-expr:$Id: test_expr.c,v 1.2 1995/08/11 15:52:42 liw Exp $"
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "publib/expr.h"
#include "publib/errormsg.h"

static int hook(const char *sym, int nargs, double *args, double *val);

int main(void) {
	char line[256];
	int i, errors;
	double result;

	set_progname(NULL, "test_expr");

	errors = 0;
	while (fgets(line, sizeof(line), stdin) != NULL) {
		i = expr_eval(line, &result, hook);
		if (i == -1) {
			errormsg(0, 0, "could not evaluate expression\n");
			++errors;
		} else
			printf("%g\n", result);
	}
	return errors ? 1 : 0;
}


static double foo(double f1, double f2) {
	return f1+f2;
}


static int hook(const char *sym, int nargs, double *args, double *val) {
	static struct {
		const char *name;
		double val;
	} vars[] = {
		{ "e",	M_E, },
		{ "pi",	M_PI, },
		{ "da",	42.0, },
		{ "x",	0, },
		{ NULL, },
	};
	static const struct {
		const char *name;
		int nargs;
		double (*fun)();	/* no args! */
	} funs[] = {
		{ "sin",	1,	sin, },
		{ "cos",	1,	cos, },
		{ "sqrt",	1,	sqrt, },
		{ "foo",	2,	foo, },
		{ NULL, },
	};
	int i;

	for (i = 0; vars[i].name != NULL; ++i) {
		if (strcmp(sym, vars[i].name) == 0) {
			if (nargs == -1)
				vars[i].val = *val;
			else if (nargs == 0)
				*val = vars[i].val;
			else {
				errormsg(0, 0, "invalid number of arguments "
					 "for %s, should be -1 or 0, is %d",
					 sym, nargs);
				return -1;
			}
			return 0;
		}
	}

	for (i = 0; funs[i].name != NULL; ++i) {
		if (strcmp(sym, funs[i].name) == 0) {
			if (nargs != funs[i].nargs) {
				errormsg(0, 0, "invalid number of arguments "
					 "for %s, should be %d, is %d",
					 sym, funs[i].nargs, nargs);
				return -1;
			}
			switch (nargs) {
			case 0:
				*val = funs[i].fun();
				break;
			case 1:
				*val = funs[i].fun(args[0]);
				break;
			case 2:
				*val = funs[i].fun(args[0], args[1]);
				break;
			case 3:
				*val = funs[i].fun(args[0], args[1], args[3]);
				break;
			default:
				/* too many args */
				return -1;
			}
			return 0;
		}
	}

	errormsg(0, 0, "unknown symbol %s", sym);
	return -1;
}
