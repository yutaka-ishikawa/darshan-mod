all: lib/libdarshan-posix.a lib/libdarshan-mpi-io.a lib/libdarshan-stubs.a lib/libdarshan-single.a

DESTDIR =
srcdir = .
prefix = /usr/local
exec_prefix = ${prefix}
datarootdir = $(DESTDIR)${prefix}/share
includedir = $(DESTDIR)${prefix}/include
mandir = $(DESTDIR)${datarootdir}/man
sbindir = $(DESTDIR)${exec_prefix}/sbin
bindir = $(DESTDIR)${exec_prefix}/bin
libdir = $(DESTDIR)${exec_prefix}/lib
LDFLAGS = 
CC = mpicc
LD = @LD@

DISABLE_LDPRELOAD = 
DARSHAN_LOG_FORMAT = $(srcdir)/../darshan-log-format.h
DARSHAN_VERSION = 2.3.0

ifndef DISABLE_LDPRELOAD
all: lib/libdarshan.so lib/libdarshan-single.so
endif



CFLAGS = -DDARSHAN_CONFIG_H=\"darshan-runtime-config.h\" -I . -I ../ -I $(srcdir) -I$(srcdir)/../ -Wall -O2 -g -DHISTORY  -D_LARGEFILE64_SOURCE 

CFLAGS_SHARED = -DDARSHAN_CONFIG_H=\"darshan-runtime-config.h\" -I . -I$(srcdir) -I$(srcdir)/../ -Wall -O2 -g -DHISTORY  -D_LARGEFILE64_SOURCE -shared -fpic -DPIC -DDARSHAN_PRELOAD

LIBS = -lz @LIBBZ2@

lib::
	@mkdir -p $@

lib/darshan-mpi-io.o: lib/darshan-mpi-io.c darshan.h darshan-dynamic.h $(DARSHAN_LOG_FORMAT) | lib
	$(CC) $(CFLAGS) -c $< -o $@

lib/darshan-mpi-io.po: lib/darshan-mpi-io.c darshan.h darshan-dynamic.h $(DARSHAN_LOG_FORMAT) | lib
	$(CC) $(CFLAGS_SHARED) -c $< -o $@

lib/darshan-mpi-init-finalize.o: lib/darshan-mpi-init-finalize.c darshan.h darshan-dynamic.h $(DARSHAN_LOG_FORMAT) | lib
	$(CC) $(CFLAGS) -c $< -o $@

lib/darshan-mpi-init-finalize.po: lib/darshan-mpi-init-finalize.c darshan.h darshan-dynamic.h $(DARSHAN_LOG_FORMAT) | lib
	$(CC) $(CFLAGS_SHARED) -c $< -o $@

lib/darshan-pnetcdf-stubs.o: lib/darshan-pnetcdf-stubs.c darshan.h $(DARSHAN_LOG_FORMAT) | lib 
	$(CC) $(CFLAGS) -c $< -o $@

lib/darshan-pnetcdf.o: lib/darshan-pnetcdf.c darshan.h $(DARSHAN_LOG_FORMAT) | lib 
	$(CC) $(CFLAGS) -c $< -o $@

lib/darshan-pnetcdf.po: lib/darshan-pnetcdf.c darshan.h $(DARSHAN_LOG_FORMAT) | lib 
	$(CC) $(CFLAGS_SHARED) -c $< -o $@

lib/darshan-hdf5.o: lib/darshan-hdf5.c darshan.h $(DARSHAN_LOG_FORMAT) | lib
	$(CC) $(CFLAGS) -c $< -o $@

lib/darshan-hdf5-stubs.o: lib/darshan-hdf5-stubs.c darshan.h $(DARSHAN_LOG_FORMAT) | lib
	$(CC) $(CFLAGS) -c $< -o $@

lib/darshan-hdf5.po: lib/darshan-hdf5.c darshan.h $(DARSHAN_LOG_FORMAT) | lib
	$(CC) $(CFLAGS_SHARED) -c $< -o $@

lib/darshan-posix.o: lib/darshan-posix.c darshan.h $(DARSHAN_LOG_FORMAT) | lib
	$(CC) $(CFLAGS) -c $< -o $@

lib/darshan-posix.po: lib/darshan-posix.c darshan.h $(DARSHAN_LOG_FORMAT) | lib
	$(CC) $(CFLAGS_SHARED) -c $< -o $@

lib/lookup3.o: lib/lookup3.c
	$(CC) $(CFLAGS) -c $< -o $@

lib/lookup3.po: lib/lookup3.c
	$(CC) $(CFLAGS_SHARED) -c $< -o $@

lib/lookup8.o: lib/lookup8.c
	$(CC) $(CFLAGS) -c $< -o $@

lib/lookup8.po: lib/lookup8.c
	$(CC) $(CFLAGS_SHARED) -c $< -o $@

%.i: %.c
	$(CC) -E $(CFLAGS) -c $< -o $@

lib/libdarshan-mpi-io.a: lib/darshan-mpi-io.o lib/darshan-mpi-init-finalize.o lib/darshan-pnetcdf.o lib/darshan-hdf5.o
	ar rcs $@ $^

lib/libdarshan-stubs.a: lib/darshan-pnetcdf-stubs.o lib/darshan-hdf5-stubs.o
	ar rcs $@ $^

lib/libdarshan-posix.a: lib/darshan-posix.o lib/lookup3.o lib/lookup8.o 
	ar rcs $@ $^

lib/libdarshan.so: lib/darshan-mpi-io.po lib/darshan-mpi-init-finalize.po lib/darshan-pnetcdf.po lib/darshan-hdf5.po lib/darshan-posix.po lib/lookup3.po lib/lookup8.po lib/darshan-history.po
	$(CC) $(CFLAGS_SHARED) $(LDFLAGS) -o $@ $^ -lpthread -lrt -lz -ldl

###############################################################################
#
CFLAGS_SINGLE_SHARED = $(CFLAGS_SHARED) -DDARSHAN_SINGLE
CFLAGS_SINGLE = $(CFLAGS) -DDARSHAN_SINGLE
lib/libdarshan-single.so: lib/darshan-single.po lib/darshan-history.po lib/darshan-posix-single.po lib/lookup3.po lib/lookup8.po
	$(CC) $(CFLAGS_SINGLE_SHARED) $(LDFLAGS) -o $@ $^ -lpthread -lrt -lz -ldl

lib/libdarshan-single.a: lib/darshan-single.o lib/darshan-history.o lib/darshan-posix-single.o lib/lookup3.o lib/lookup8.o
	ar rcs $@ $^

lib/darshan-single.o: lib/darshan-single.c darshan.h darshan-dynamic.h $(DARSHAN_LOG_FORMA) | lib
	$(CC) $(CFLAGS_SINGLE) -c $< -o $@

lib/darshan-single.po: lib/darshan-single.c darshan.h darshan-dynamic.h $(DARSHAN_LOG_FORMA) | lib
	$(CC) $(CFLAGS_SINGLE_SHARED) -c $< -o $@

lib/darshan-history.o: lib/darshan-history.c darshan.h darshan-dynamic.h $(DARSHAN_LOG_FORMA) | lib
	$(CC) $(CFLAGS) -c $< -o $@

lib/darshan-history.po: lib/darshan-history.c darshan.h darshan-dynamic.h $(DARSHAN_LOG_FORMA) | lib
	$(CC) $(CFLAGS) $(CFLAGS_SHARED) -c $< -o $@

lib/darshan-posix-single.o: lib/darshan-posix.c darshan.h $(DARSHAN_LOG_FORMAT) | lib
	$(CC) $(CFLAGS_SINGLE) -c $< -o $@

lib/darshan-posix-single.po: lib/darshan-posix.c darshan.h $(DARSHAN_LOG_FORMAT) | lib
	$(CC) $(CFLAGS_SINGLE_SHARED) -c $< -o $@
###############################################################################

install:: all
	install -d $(libdir)
	install -m 755 lib/libdarshan-posix.a $(libdir)
	install -m 755 lib/libdarshan-mpi-io.a $(libdir)
	install -m 755 lib/libdarshan-stubs.a $(libdir)
ifndef DISABLE_LDPRELOAD
	install -m 755 lib/libdarshan.so $(libdir)
endif
	install -d $(bindir)
	install -m 755 darshan-mk-log-dirs.pl $(bindir)
	install -m 755 darshan-gen-cc.pl $(bindir)
	install -m 755 darshan-gen-cxx.pl $(bindir)
	install -m 755 darshan-gen-fortran.pl $(bindir)
	install -m 755 darshan-config $(bindir)
	install -d $(datarootdir)/craype-1.x/modulefiles/darshan
	install -m 755 share/craype-1.x/darshan-module $(datarootdir)/craype-1.x/modulefiles/darshan/$(DARSHAN_VERSION)
	install -d $(datarootdir)/craype-2.x/modulefiles/darshan
	install -m 755 share/craype-2.x/darshan-module $(datarootdir)/craype-2.x/modulefiles/darshan/$(DARSHAN_VERSION)
	install -d $(libdir)/pkgconfig
	install -m 644 lib/pkgconfig/darshan-runtime.pc $(libdir)/pkgconfig/darshan-runtime.pc

clean::
	rm -f *.o *.a lib/*.o lib/*.po lib/*.a lib/*.so 

distclean:: clean
	rm -f darshan-runtime-config.h darshan-gen-cxx.pl darshan-gen-fortran.pl darshan-gen-cc.pl darshan-mk-log-dirs.pl aclocal.m4 autom4te.cache/* config.status config.log Makefile 
	rm -rf autom4te.cache
