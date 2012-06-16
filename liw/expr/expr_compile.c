/*
 * expr_compile.c -- compile expression given in string form
 *
 * Part of publib.  See man page for more information
 * "@(#)publib-expr:$Id: expr_compile.c,v 1.1.1.1 1993/11/20 15:57:29 liw Exp $"
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "publib/expr.h"
#include "publib/errormsg.h"


#define UNGOT_MAX	8
static struct expr_instr ungot_tokens[UNGOT_MAX];
static int ungot_count = 0;


static int get_token(const char **s, struct expr_instr *in);
static void unget_token(struct expr_instr *in);
static int assignment(const char **s, Expr *e);
static int expr(const char **s, Expr *e);
static int factor(const char **s, Expr *e);
static int term(const char **s, Expr *e);
static int simple_expr(const char **s, Expr *e);
static int arg_list(struct expr_instr *syminstr, const char **s, Expr *e);
static int push(Expr *e, struct expr_instr *);



Expr *expr_compile(const char *string) {
	Expr *e;

	e = (Expr *) malloc(sizeof(Expr));
	if (e == NULL) {
		__publib_error("malloc failed");
		return NULL;
	}

	e->prg = NULL;
	e->size = 0;
	e->allocated = 0;
	if (assignment(&string, e) == -1) {
		expr_free(e);
		return NULL;
	}
	if (*string != '\0') {
		__publib_error("syntax error: "
			       "expression continues unexpectedly");
		expr_free(e);
		return NULL;
	}
	return e;
}

static int assignment(const char **s, Expr *e) {
	struct expr_instr in, in2;

	if (get_token(s, &in) == -1)
		return -1;
	if (in.op != op_sym || get_token(s, &in2) == -1) {
		unget_token(&in);
		return expr(s, e);
	}

	if (in2.op != op_ass) {
		unget_token(&in2);
		unget_token(&in);
		return expr(s, e);
	}

	in2.val.sym = in.val.sym;
	if (expr(s, e) == -1 || push(e, &in2) == -1)
		return -1;
	return 0;
}


static int expr(const char **s, Expr *e) {
	struct expr_instr in;

	if (term(s, e) == -1)
		return -1;
	while (get_token(s, &in) != -1) {
		if (in.op != op_add && in.op != op_sub) {
			unget_token(&in);
			return 0;
		}
		if (term(s, e) == -1 || push(e, &in) == -1)
			return -1;
	}
	return 0;
}


static int term(const char **s, Expr *e) {
	struct expr_instr in;

	if (factor(s, e) == -1)
		return -1;
	while (get_token(s, &in) != -1) {
		if (in.op != op_mul && in.op != op_div && in.op != op_rem) {
			unget_token(&in);
			return 0;
		}
		if (factor(s, e) == -1 || push(e, &in) == -1)
			return -1;
	}
	return 0;
}


static int factor(const char **s, Expr *e) {
	struct expr_instr in;

	if (get_token(s, &in) == -1)
		return -1;
	if (in.op == op_sub || in.op == op_add) {
		if (get_token(s, &in) == -1)
			return -1;
		if (in.op != op_num) {
			__publib_error("syntax error: "
				       "unary + or - not followed by number");
			return -1;
		}
		in.op = op_num;
		if (in.op == op_sub)
			in.val.num = -in.val.num;
		else
			in.val.num = in.val.num;
		return 0;
	}

	unget_token(&in);
	return simple_expr(s, e);
}


static int simple_expr(const char **s, Expr *e) {
	struct expr_instr in;

	if (get_token(s, &in) == -1)
		return -1;

	switch (in.op) {
	case op_num:
		if (push(e, &in) == -1)
			return -1;
		return 0;
		
	case op_left:
		if (expr(s, e) == -1)
			return -1;
		if (get_token(s, &in) == -1)
			return -1;
		if (in.op != op_right) {
			__publib_error("syntax error: unbalanced (");
			return -1;
		}
		return 0;

	case op_sym:
		if (arg_list(&in, s, e) == -1)
			return -1;
		if (push(e, &in) == -1)
			return -1;
		return 0;

	default:
		__publib_error("syntax error: unexpected token");
		return -1;
	}
}


static int arg_list(struct expr_instr *syminstr, const char **s, Expr *e) {
	struct expr_instr in;

	if (get_token(s, &in) == -1)
		return 0;

	if (in.op != op_left) {
		unget_token(&in);
		return 0;
	}

	syminstr->op = op_fun;
	syminstr->val.fun.name = syminstr->val.sym;
	syminstr->val.fun.nargs = 0;

	do {
		if (expr(s, e) == -1)
			return -1;
		if (get_token(s, &in) == -1)
			return -1;
		if (in.op != op_right && in.op != op_comma) {
			__publib_error("syntax error: expression not "
				 "followed by , or ) in argument list");
			return -1;
		}
		++syminstr->val.fun.nargs;
	} while (in.op == op_comma);

	return 0;
}


static void unget_token(struct expr_instr *in) {
	ungot_tokens[ungot_count] = *in;
	if (ungot_count < UNGOT_MAX)
		++ungot_count;
}


static int get_token(const char **s, struct expr_instr *in) {
	int off = 1;

	if (ungot_count > 0) {
		--ungot_count;
		*in = ungot_tokens[ungot_count];
		return 0;
	}

	while (isspace(**s))
		++(*s);

	switch (**s) {
	case '+':
		in->op = op_add;
		break;

	case '-':
		in->op = op_sub;
		break;

	case '*':
		in->op = op_mul;
		break;

	case '/':
		in->op = op_div;
		break;

	case '%':
		in->op = op_rem;
		break;

	case '(':
		in->op = op_left;
		break;

	case ')':
		in->op = op_right;
		break;

	case ',':
		in->op = op_comma;
		break;

	case '=':
		in->op = op_ass;
		break;

	case '\0':
		return -1;

	default:
		if (isalpha(**s) || **s == '_') {
			char *sym;
			const char *p = *s;
			size_t len;
			while (isalnum(**s) || **s == '_')
				++(*s);
			len = *s - p + 1;
			sym = (char *) malloc(len);
			if (sym == NULL) {
				__publib_error("malloc failed");
				return -1;
			}
			memcpy(sym, p, len-1);
			sym[len-1] = '\0';
			in->op = op_sym;
			in->val.sym = sym;
			off = 0;
			break;
		}

		if (!isdigit(**s)) {
			__publib_error("syntax error: unexpected character");
			return -1;
		}

		in->op = op_num;
		in->val.num = strtod(*s, (char **)s);
		off = 0;
		break;
	}

	*s += off;
	return 0;
}


static int push(Expr *e, struct expr_instr *in) {
	struct expr_instr *p;
	size_t n;

	if (e->size >= e->allocated) {
		n = e->allocated + 64;
		if (e->prg == NULL)
			p = (struct expr_instr *)malloc(n*sizeof(*p));
		else
			p = (struct expr_instr *)realloc(e->prg, n*sizeof(*p));
		if (p == NULL) {
			__publib_error("realloc failed");
			return -1;
		}
		e->allocated = n;
		e->prg = p;
	}
	e->prg[e->size] = *in;
	++e->size;
	return 0;
}
