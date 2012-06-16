#ifndef __publib_expr_h
#define __publib_expr_h

enum expr_opcode {
	op_add, op_sub, op_mul, op_div, op_rem, op_num, op_sym,	op_fun,
	op_ass,

	/* the rest are not real operands, but rather tokens (used during
	   parsing; they mustn't be in the executing expression)  */
	op_left, op_right, op_comma
};
 
struct expr_instr {
	enum expr_opcode op;
	union {
		double num;
		char *sym;
		struct {
			int nargs;
			char *name;
		} fun;
	} val;
};

struct expr_code {
	struct expr_instr *prg;
	size_t size, allocated;
};
typedef struct expr_code Expr;

typedef int expr_hook(const char *, int __n, double *__args, double *__val);

int expr_eval(const char *__expr, double *__res, expr_hook *__hook);
Expr *expr_compile(const char *__source);
int expr_run(Expr *__compiled, double *__res, expr_hook *__hook);
void expr_free(Expr *);

#endif
