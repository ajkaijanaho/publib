objs = priq.o
testprogs = test-priq
include $(fwdir)/Generic.mk

test-priq: test-priq.o $(libname)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ test-priq.o $(libname)
test-priq.o: priq.h
$(objs): priq.h
