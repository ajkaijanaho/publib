objs = main_filter.o
testprogs = test-main-filter
include $(fwdir)/Generic.mk

test-main-filter: test-main-filter.o $(libname)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ test-main-filter.o $(libname) -lc
test-main-filter.o: main.h
$(objs): main.h
