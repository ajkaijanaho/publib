objs =  iset_add.o \
	iset_add_rng.o \
	iset_clear.o \
	iset_contains.o \
	iset_copy.o \
	iset_create.o \
	iset_destroy.o \
	iset_diff.o \
	iset_is_empty.o \
	iset_isect.o \
	iset_nth_rng.o \
	iset_range.o \
	iset_remove.o \
	iset_rm_rng.o \
	iset_union.o 
testprogs = test_add
include $(fwdir)/Generic.mk

test_add: test_add.o $(libname)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ test_add.o $(libname)

test_add.o: iset.h
$(objs): iset.h
