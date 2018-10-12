#!/usr/bin/env python

import multiprocessing
import os

n = 4

def worker(x):
    print('%d: worker-%d' % (os.getpid(), x))
    #print(os.getenv('LD_PRELOAD'))
    f = open(str(x) + '.txt', 'w')
    f.write('0123456789')
    f.close()


if __name__ == '__main__':
    #multiprocessing.set_start_method('spawn')
    #multiprocessing.set_start_method('fork')
    #os.environ['LD_PRELOAD'] = './libdarshan-single.so'
    #print(os.getenv('LD_PRELOAD'))
    print('%d: master' % os.getpid())
    f = open('9.txt', 'w')
    f.write('0123456789')
    f.close()
    pool = multiprocessing.Pool(n)
    print(pool)
    pool.map(worker, range(n))
    print('END')
    
