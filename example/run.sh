#!/bin/bash
#
#PJM --rsc-list rscgrp=small
#PJM --rsc-list node=12
#PJM --rsc-list elapse=0:10:00  # HH:MM:SS
#PJM --mpi use-rankdir
#
#PJM --stgin "./simple ./"
#PJM --stgin "/home/ra000022/a03228/work/darshan/darshan-2.3.0/darshan-runtime/lib/libdarshan.so ./"
#PJM --stgout-dir "../darshan/ ./results/%n.d%j/job/darshan/ stgout=all,recursive=99"
#

. /work/system/Env_base

mkdir -p ../darshan/`date +%Y/%m/%d | sed -e "s/\/0/\//g"`

mpiexec -x "LD_PRELOAD=./libdarshan.so" -ofprefix rank,nid ./simple wdata

