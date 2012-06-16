objs =  expr_compile.o \
	expr_eval.o \
	expr_free.o \
	expr_run.o
testprogs = test_expr
include $(fwdir)/Generic.mk

test_expr: test_expr.o $(libname)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ test_expr.o $(libname) -lm

test_expr.o: expr.h 
$(objs): expr.h 
