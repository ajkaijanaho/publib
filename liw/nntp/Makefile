objs = nntp.o nntpart.o
testprogs = 
include $(fwdir)/Generic.mk

xxxtest: xxxtest.o $(libname)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ xxxtest.o $(libname)
xxxtest.o: nntp.h
$(objs): nntp.h 
