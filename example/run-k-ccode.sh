#!/bin/bash
#
#PJM --rsc-list rscgrp=small
#PJM --rsc-list node=1
#PJM --rsc-list elapse=0:10:00  # HH:MM:SS
#PJM --mpi use-rankdir
#
#PJM --stgin "./ccode ./"
#PJM --stgin "../k-lib/libdarshan-single.so ./"
#PJM --stgout "./darshanlog-* ./results/"  
#

. /work/system/Env_base

(export LD_PRELOAD=./libdarshan-single.so; export DARSHAN_HISTORY_RW=rw; ./ccode a1 a2 a3 a4)
