objs = queue.o
testprogs = test-queue
include $(fwdir)/Generic.mk

test-queue: test-queue.o $(libname)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ test-queue.o $(libname)
test-queue.o: queue.h
$(objs): queue.h
