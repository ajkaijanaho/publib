objs =  fnbase.o \
	fndelbeg.o \
	fndelend.o \
	fndelsuf.o \
	fndir.o \
	fnhome.o \
	fnjoin.o \
	fnlastsuf.o \
	fnpathfind.o \
	fnqualify.o \
	fnsetsuf.o
testprogs = test_fnqualify
include $(fwdir)/Generic.mk

test_fnqualify: test_fnqualify.o $(libname)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ test_fnqualify.o $(libname)

test_fnqualify.o: fname.h
$(objs): fname.h
