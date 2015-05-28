#MPICC   =       mpifccpx
#CFLAGS  = -DFJ_MPI -O3 -Xg
MPICC = mpicc
CC = cc
CFLAGS  = -O3 -Wall

all: simple simple2 simple3
simple: simple.c
	$(MPICC) $(CFLAGS) -o simple simple.c
simple2: simple2.c
	$(MPICC) $(CFLAGS) -o simple2 simple2.c
simple3: simple3.c
	$(MPICC) $(CFLAGS) -o simple3 simple3.c
ccode: ccode.c
	$(CC) $(CFLAGS) -o ccode ccode.c
clean:
	rm -f wdata* *.o simple simple2 simple3
sub:
	pjsub ./run.sh
stat:
	pjstat
tar:
	tar czf test.tar.gz Makefile Makefile.example *.c run-k.sh run-oakleaf.sh
rcp:
	scp test.tar.gz a03228@k.aics.riken.jp:
myrun:
	mkdir -p ./darshan/`date +%Y/%m/%d | sed -e "s/\/0/\//g"`
	export LD_PRELOAD=./libdarshan.so; export DARSHAN_HISTORY_RW="w"; mpirun -np 2 ./simple wdata
myrun2:
	mkdir -p ./darshan/`date +%Y/%m/%d | sed -e "s/\/0/\//g"`
	export LD_PRELOAD=./libdarshan.so; export DARSHAN_HISTORY_RW="rw"; export DARSHAN_HISTORY_MEMSIZE=10485760; mpirun -np 2 ./simple2 wdata
	export LD_PRELOAD=./libdarshan.so; export DARSHAN_HISTORY_RW="rw"; export DARSHAN_HISTORY_MEMSIZE=10485760; mpirun -np 2 ./simple3 wdata

myrunc:
	export LD_PRELOAD=./libdarshan-single.so; export DARSHAN_HISTORY_RW="w"; ./ccode a1 a2 a3 a4
top:
	export LD_PRELOAD=./libdarshan-single.so; export DARSHAN_HISTORY_RW="rw"; top
libcopy:
	cp -p ../darshan-2.3.0/darshan-runtime/lib/libdarshan.so .
	cp -p ../darshan-2.3.0/darshan-runtime/lib/libdarshan-single.so .

#	export LD_PRELOAD=./libdarshan.so; export DARSHAN_HISTORY_RW="w"; mpirun -np 2 ./simple2 wdata
#
#	export LD_PRELOAD=./libdarshan.so; mpirun -np 2 ./simple wdata
#
#	darshan-parser <log-file.gz>
#	darshan-analyzer <log-file.gz>
#	darshan-summary-per-file <log-file.gz> <directory_name>
#	darshan-summary-job-summary.pl <log-file.gz>

