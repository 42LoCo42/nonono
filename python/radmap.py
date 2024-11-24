#!/usr/bin/env python3
import random
import numpy as np
from collections import namedtuple

maxY = 16
maxOther = 32

Point = namedtuple("Point", "x y z")
Direction = namedtuple("Direction", "theta phi")

def randomPoint():
    return Point(
        random.randrange(0, maxOther),
        random.randrange(0, maxY),
        random.randrange(0, maxOther),
    )

def calcDirection(point):
    theta = np.arccos(
        point.z / np.sqrt(sum(map(lambda x: x**2, point)))
    )
    phi = np.sign(point.y) * np.arccos(
        point.x / np.sqrt(point.x ** 2 + point.y ** 2)
    )
    return Direction(theta, phi)
