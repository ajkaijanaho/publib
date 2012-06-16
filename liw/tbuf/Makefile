objs = tbuf.o
testprogs = test_tbuf test_tbuf_with_stats
include $(fwdir)/Generic.mk

test_tbuf: test_tbuf.o $(libname)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(objdir)/$@ $(objdir)/test_tbuf.o \
		$(libname) $(LIBS)

test_tbuf_with_stats: test_tbuf_with_stats.o $(libname)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(objdir)/$@ \
		$(objdir)/test_tbuf_with_stats.o $(libname) $(LIBS)

test_tbuf.o: tbuf.h 
$(objs): tbuf.h
