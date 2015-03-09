#!/usr/bin/env python
from random import randint

for i in xrange(1000000):
    print '{}.{}.{}.{}'.format(randint(0, 255), randint(0, 255), randint(0, 255), randint(0, 255))
