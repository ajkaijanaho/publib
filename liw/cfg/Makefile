objs = cfg.o
testprogs = test-cfg
include $(fwdir)/Generic.mk

test-cfg: test-cfg.o $(libname)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(objdir)/$@ test-cfg.o $(libname) -lm -lc
test-cfg.o: cfg.h
$(objs): cfg.c cfg.h
