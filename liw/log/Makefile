objs = log.o
testprogs = test-log
include $(fwdir)/Generic.mk

test-log: test-log.o $(libname)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ test-log.o $(libname)

test-log.o: log.h
$(objs): log.h
