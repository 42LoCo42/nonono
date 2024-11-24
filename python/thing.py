from random import randint

LOWER = -8
UPPER =  8

def rand_except(e):
    r = randint(LOWER, UPPER) * 2
    while r == e:
        r = randint(LOWER, UPPER) * 2
    return r

def gen():
    x1 = randint(LOWER, UPPER) * 2
    x2 = rand_except(x1)
    y  = randint(LOWER, UPPER) * 2
    params = [
        0 - x1 - x2,
        (-x1)*(-x2) + y,
        0,
        0
    ]
    params[2] = params[0] / 2
    params[3] = params[1] - pow(params[2], 2)
    print(f"({x1}|{y}) und ({x2}|{y})	    p,q,d,e = {params}")

for _ in range(0, 5):
    gen()
