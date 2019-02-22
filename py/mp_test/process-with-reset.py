#!/usr/bin/env python

import darshan
import multiprocessing
import os

n = 4

def worker(x):
    darshan.reset('worker-%d' % x)
    print('%d: worker-%d' % (os.getpid(), x))
    f = open(str(x) + '.txt', 'w')
    f.write('0123456789')
    f.close()


if __name__ == '__main__':
    print('%d: master' % os.getpid())
    f = open('9.txt', 'w')
    f.write('0123456789')
    f.close()
    jobs = []
    for i in range(n):
        job = multiprocessing.Process(target=worker, args=(i,))
        job.start()
        jobs.append(job)
    for job in jobs:
        job.join()

