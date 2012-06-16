objs = xxx.o
testprogs = xxxtest
include $(fwdir)/Generic.mk

xxxtest: xxxtest.o $(libname)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ xxxtest.o $(libname)
xxxtest.o: xxx.h
$(objs): xxx.h
