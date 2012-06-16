objs = sbuf.o sbuf_aux.o sbuf_srch.o
testprogs = test-sbuf
include $(fwdir)/Generic.mk

test-sbuf: test-sbuf.o $(libname)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(objdir)/$@ $(objdir)/test-sbuf.o \
		$(libname)

test-sbuf.o: sbuf.h 
$(objs): sbuf.h
