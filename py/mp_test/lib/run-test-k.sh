#!/bin/bash
#
#PJM --rsc-list rscgrp=small
#PJM --rsc-list node=1
#PJM --rsc-list elapse=0:10:00  # HH:MM:SS
#PJM --mpi use-rankdir
#PJM --stgin "./darshantest.py ./"
#PJM --stgin "./darshan.so ./"

. /work/system/Env_base

python ./darshantest.py


