DARSHAN_SRCTOP=../../
ifeq (`which fccpx`,)
CC = cc
else
CC = fccpx
CFLAGS += -Xg
endif

tsctest: tsctest.c tsc.h
	$(CC) $(CFLAGS) -O2 -o tsctest tsctest.c
clonetest: clonetest.c
	cc -g -O2 -c clonetest.c
	cc -g -o clonetest clonetest.o
forktest: forktest.c
	cc -g -O2 -c forktest.c
	cc -g -o forktest forktest.o
runtest:
	export LD_PRELOAD=./libdarshan-single.so; export DARSHAN_HISTORY_RW="rw"; export DARSHAN_HISTORY_MEMSIZE=1048576000; ./clonetest
runtest2:
	export LD_PRELOAD=./libdarshan-single.so; export DARSHAN_HISTORY_RW="rw"; export DARSHAN_HISTORY_MEMSIZE=1048576000; ./forktest
run:
	export LD_PRELOAD=./libdarshan-single.so; export DARSHAN_HISTORY_RW="rw"; export DARSHAN_HISTORY_MEMSIZE=1048576000; ./serial.py
runpool:
	export LD_PRELOAD=./libdarshan-single.so; export DARSHAN_HISTORY_RW="rw"; export DARSHAN_HISTORY_MEMSIZE=1048576000; python3 ./pool.py
runprocess:
	export LD_PRELOAD=./libdarshan-single.so; export DARSHAN_HISTORY_RW="rw"; export DARSHAN_HISTORY_MEMSIZE=1048576000; python3 ./process.py
clean:
	rm -f *gz run-tsc-k.sh.{e,o,s}*  run-k.sh.{e,o,s}*
clean2:
	rm -f tsctest clonetest forktest
copy:
	cp -p $(DARSHAN_SRCTOP)/darshan-2.3.0/darshan-runtime/lib/libdarshan-single.so .
examine:
	for i in darshanlog-*gz; do\
		darshan-parser --history $$i | grep mp_test;\
	done
