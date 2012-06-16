/*
 * expr_run.c -- run a compiled expression
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-expr:$Id: expr_run.c,v 1.1.1.1 1993/11/20 15:57:29 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "publib/expr.h"
#include "publib/errormsg.h"

static int grow(size_t, double **stack, size_t *stsize);

int expr_run(Expr *e, double *res, expr_hook *hook) {
	int pc;
	double *stack, result;
	size_t top, stsize;
	struct expr_instr *in;

	stack = NULL;
	stsize = top = 0;
	for (pc = 0; pc < e->size; ++pc) {
		in = &e->prg[pc];
		switch (in->op) {
		case op_ass:
			assert(top > 0);
			if (hook(in->val.sym, -1, 0, stack+top-1) == -1) {
				__publib_error("hook failed");
				return -1;
			}
			break;

		case op_add:
			stack[top-2] += stack[top-1];
			--top;
			break;

		case op_sub:
			stack[top-2] -= stack[top-1];
			--top;
			break;

		case op_mul:
			stack[top-2] *= stack[top-1];
			--top;
			break;

		case op_div:
			stack[top-2] /= stack[top-1];
			--top;
			break;

		case op_rem:
			stack[top-2] = fmod(stack[top-2], stack[top-1]);
			--top;
			break;

		case op_num:
			if (top >= stsize && grow(64, &stack, &stsize) == -1)
				return -1;
			stack[top] = in->val.num;
			++top;
			break;

		case op_sym:
			if (grow(1, &stack, &stsize) == -1)
				return -1;
			if (hook(in->val.sym, 0, 0, stack+top) == -1) {
				__publib_error("hook failed");
				return -1;
			}
			++top;
			break;

		case op_fun:
			if (grow(1, &stack, &stsize) == -1)
				return -1;
			if (hook(in->val.fun.name, in->val.fun.nargs,
				 stack+top-in->val.fun.nargs, &result)==-1) {
				__publib_error("hook failed");
				return -1;
			}
			top -= in->val.fun.nargs;
			stack[top] = result;
			++top;
			break;

		default:
			__publib_error("invalid stack code");
			return -1;
		}
	}

	assert(top == 1);
	*res = stack[0];
	return 0;
}



static int grow(size_t inc, double **stack, size_t *size) {
	size_t newsize;
	double *st;

	newsize = *size + inc;
	if (*stack == NULL)
		st = (double *) malloc(newsize * sizeof(double));
	else
		st = (double *) realloc(*stack, newsize * sizeof(double));
	if (st == NULL) {
		free(*stack);
		__publib_error("realloc failed");
		return -1;
	}

	*size = newsize;
	*stack = st;

	return 0;
}
