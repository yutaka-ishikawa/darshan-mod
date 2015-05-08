#!/bin/bash                                                                     
#PJM --rsc-list rscgrp=small                                                    
#PJM --rsc-list node=12                                                         
#PJM -g ge29                                                                    
#PJM --rsc-list elapse=0:10:00                                                  

mkdir -p ../darshan/`date +%Y/%m/%d | sed -e "s/\/0/\//"`

mpiexec -x "LD_PRELOAD=./libdarshan.so" -x "DARSHAN_HISTORY_RW=w" -ofprefix rank,nid ./simple wdata

#mpiexec -x "LD_PRELOAD=./libdarshan.so" -x "DARSHAN_HISTORY_RW=w"-ofprefix rank,nid ./simple wdata                                                     
