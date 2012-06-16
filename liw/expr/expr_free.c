/*
 * expr_free.c -- free resources allocated for compiled expression
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-expr:$Id: expr_free.c,v 1.1.1.1 1993/11/20 15:57:29 liw Exp $"
 */

#include <assert.h>
#include <stdlib.h>
#include "publib/expr.h"

void expr_free(Expr *e) {
	size_t i;

	for (i = 0; i < e->size; ++i) {
		switch (e->prg[i].op) {
		case op_sym:
			free(e->prg[i].val.sym);
			break;
		case op_fun:
			free(e->prg[i].val.fun.name);
			break;
		default:
			/* ignored */
			break;
		}
	}
	free(e->prg);
	free(e);
}
