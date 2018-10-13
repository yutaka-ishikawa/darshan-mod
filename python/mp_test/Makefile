DARSHAN_SRCTOP=../../
CC := $(shell which fccpx)
ifeq ($(strip $(CC)),)
CC = cc
else
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
###
runpool:
	export LD_PRELOAD=./libdarshan-single.so; export PYTHONPATH=./lib; export DARSHAN_HISTORY_RW="rw"; export DARSHAN_HISTORY_MEMSIZE=1048576000; python3 ./pool.py
runpool-with-reset:
	export LD_PRELOAD=./libdarshan-single.so; export PYTHONPATH=./lib; export DARSHAN_HISTORY_RW="rw"; export DARSHAN_HISTORY_MEMSIZE=1048576000; python3 ./pool-with-reset.py
###
runprocess:
	export LD_PRELOAD=./libdarshan-single.so;  export PYTHONPATH=./lib; export DARSHAN_HISTORY_RW="rw"; export DARSHAN_HISTORY_MEMSIZE=1048576000; python3 ./process.py
runprocess-with-reset:
	export LD_PRELOAD=./libdarshan-single.so;  export PYTHONPATH=./lib; export DARSHAN_HISTORY_RW="rw"; export DARSHAN_HISTORY_MEMSIZE=1048576000; python3 ./process-with-reset.py
clean:
	rm -f *gz run-tsc-k.sh.{e,o,s}*  run-*-k.sh.{e,o,s}* ?.txt
	rm -rf POOL POOL-WITH-RESET PROCESS PROCESS-WITH-RESET
clean2:
	rm -f tsctest clonetest forktest
copy:
	cp -p $(DARSHAN_SRCTOP)/darshan-2.3.0/darshan-runtime/lib/libdarshan-single.so .
examine:
	for i in darshanlog-*gz; do\
		darshan-parser --history $$i | grep mp_test;\
	done
examine2:
	for d in POOL* PROCESS*; do\
	  echo $$d;\
	  for i in $$d/darshanlog-*gz; do\
		darshan-parser --history $$i | grep txt;\
	  done;\
	done;

