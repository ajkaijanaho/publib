/*
 * expr_eval.c -- evaluate expression given in string form
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-expr:$Id: expr_eval.c,v 1.1.1.1 1993/11/20 15:57:29 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include "publib/expr.h"

int expr_eval(const char *string, double *res, expr_hook *hook) {
	Expr *e;
	int ret;

	e = expr_compile(string);
	if (e == NULL)
		return -1;
	ret = expr_run(e, res, hook);
	expr_free(e);
	return ret;
}
