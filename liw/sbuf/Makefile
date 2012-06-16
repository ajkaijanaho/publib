objs = sbuf.o sbuf_aux.o sbuf_srch.o sbuf_io.o
testprogs = test-sbuf test-colmark test-sbuf-undo
include $(fwdir)/Generic.mk

test-sbuf: test-sbuf.o $(libname)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(objdir)/$@ $(objdir)/test-sbuf.o \
		$(libname)

test-sbuf-undo: test-sbuf-undo.o $(libname)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(objdir)/$@ $(objdir)/test-sbuf-undo.o \
		$(libname)

test-colmark: test-colmark.o $(libname)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(objdir)/$@ $(objdir)/test-colmark.o \
		$(libname)

test-sbuf.o test-sbuf-undo.o test-colmark.o: sbuf.h 
$(objs): sbuf.h
