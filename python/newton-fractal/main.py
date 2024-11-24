#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np
import sympy as sym
from time import time

launchtime = time()
roots = [
    # +5 + 0j,
    # -1 + 1j,
    # -1 - 1j,
    1.4 + 6.2j,
    np.pi + np.pi * 1j,
    np.e + np.e * 1j,
    3.78 + 1.2j,
    6 + 9j,
]
step = 0.1


def log(*args):
    print(
        f"{time() - launchtime :.6f}s:",
        *args,
    )


roots = np.array(roots)
x = sym.Symbol("x")
f = sym.Integer(1)
[f := f * (x - r) for r in roots]
f = sym.simplify(f)
log("function:", f)

N = sym.simplify(x - f / sym.diff(f, x))
log("newton:  ", N)

N = sym.lambdify(x, N)

reals = np.arange(-960 * step, 960 * step, step)  # 1920/2
imags = np.arange(-540 * step, 540 * step, step)  # 1080/2
xs = (imags * 1j)[:, None] + reals
ys = N(N(N(N(N(N(N(N(N(N(N(N(xs))))))))))))
log(f"calculated {ys.size} values")


def colorize(y):
    if abs(y) >= 20:
        return 0

    return sorted(
        map(
            lambda r, ix: (ix, abs(y - r)),
            roots,
            range(1, len(roots) + 1),
        ),
        key=lambda i: i[1],
    )[0][0]


log("now rendering, TODO outsource to GPU or smth")
pixels = np.vectorize(colorize)(ys)
plt.imsave("fractal.png", pixels)
