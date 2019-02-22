#!/usr/bin/env python

import os

n = 4

def worker(x):
    print('%d: worker-%d' % (os.getpid(), x))
    f = open(str(x) + '.txt', 'w')
    f.write('0123456789')
    f.close()


if __name__ == '__main__':
    print('%d: master' % os.getpid())
    for i in range(n):
        worker(i)
