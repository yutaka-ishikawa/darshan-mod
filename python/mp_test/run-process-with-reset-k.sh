#!/bin/bash
#
#PJM --rsc-list rscgrp=small
#PJM --rsc-list node=1
#PJM --rsc-list elapse=0:10:00  # HH:MM:SS
#PJM --mpi use-rankdir

#PJM --stgin "./process-with-reset.py ./"
#PJM --stgin "./libdarshan-single.so ./"
#PJM --stgin-dir "./lib ./"
#PJM --stgout-dir "./PROCESS-WITH-RESET ./PROCESS-WITH-RESET"

. /work/system/Env_base

export PYTHONPATH=./; \
export LD_PRELOAD=./libdarshan-single.so; \
export DARSHAN_SINGLE_LOG_DIR=./PROCESS-WITH-RESET; \
export DARSHAN_HISTORY_RW="rw"; \
export DARSHAN_HISTORY_MEMSIZE=1048576000; \
./process-with-reset.py

ls -ld
