# This is a generic makefile for publib modules.  The module's makefile will
# look like this:
#
#	objs = xxx.o
#	include $(srcdir)/Generic.mk
#
#	$(objs): xxx.h
#
# and this included file will take care of the rest.  It can also define
# the macro testprogs, which lists test program target names, e.g.,
#
#	objs = xxx.o
#	testprogs = xxxtest
#	include $(srcdir)/Generic.mk
#	
#	xxxtest: xxxtest.o $(libname)
#		$(CC) $(CFLAGS) $(LDFLAGS) -o $@ xxxtest.o $(libname)
#	xxxtest.o: xxx.h
#	$(objs): xxx.h
#
# (you can copy modules/Template/Makefile instead of pasting from here).
#
# However, this is rather simple, and if a module needs more than this
# generic makefile provides, it will have to do things by itself, by
# not including this file, and instead doing the same things itself.
# It needs to have at least all the targets in this makefile, and will
# have to include $(srcdir)/Rules.mk.
#
# "@(#)publib-framework:$Id: Generic.mk,v 1.3 1995/09/23 09:09:35 liw Exp $"

include $(objdir)/Rules.mk

all: $(libname)
	@true
$(libname): $(objs)

tests: $(testprogs)
	@true

clean realclean distclean:
	rm -f *.o $(testprogs) a.out core

pre-install post-install:
	@true
install:
	@for i in $(srcdir)/*.3; do \
		if test -f $$i; then \
		$(INSTALL_MAN) $$i $(man3dir)/`basename $$i .3`.$(man3ext); \
		fi; \
	done
