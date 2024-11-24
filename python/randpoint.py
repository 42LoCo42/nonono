#!/usr/bin/env python3
from random import uniform
from math import sqrt

def randpoint(lo, hi):
    return (uniform(lo, hi), uniform(lo, hi))

def dist(p1, p2):
    return sqrt((p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2)

def avg(xs):
    return sum(xs) / len(xs)

rp = lambda: randpoint(0, 1)
dists = [dist(rp(), rp()) for _ in range(1000000)]
print(avg(dists))
