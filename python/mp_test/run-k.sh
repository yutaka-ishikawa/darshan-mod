#!/bin/bash
#
#PJM --rsc-list rscgrp=small
#PJM --rsc-list node=12
#PJM --rsc-list elapse=0:10:00  # HH:MM:SS
#PJM --mpi use-rankdir

#PJM --stgin "./pool.py ./"
#PJM --stgin "./libdarshan-single.so ./"
#PJM --stgin-dir "./lib ./"

. /work/system/Env_base

which perl
which perl3

export PYTHONPATH=./lib

perl --version
./pool.py

