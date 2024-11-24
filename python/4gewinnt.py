C = 0
R = 1
Y = 2

board = [
[C, C, C, C, C, C, C],
[C, C, C, C, C, C, C],
[C, R, C, R, R, R, R],
[C, R, Y, Y, Y, Y, R],
[C, R, Y, Y, R, Y, R],
[R, Y, Y, R, R, Y, R]
]

def at(list, ix):
    return None if ix < 0 or ix >= len(list) else list[ix]

def generalized_at(multidim, *args):
    val = multidim
    for ix in args:
        val = at(val, ix)
        if val == None:
            return None
    return val

def generalized_slice(multidim, startixes, increments, count):
    data = []
    for c in range(0, count):
        data.append(generalized_at(multidim, *startixes))
        startixes = list(map(sum, zip(startixes, increments)))
    return data

def all_len4_slices(multidim, x, y):
    slices = []
    for i in range(-3, 1):
        slices.append(generalized_slice(multidim, [y, x + i], [0, 1], 4))
        slices.append(generalized_slice(multidim, [y + i, x], [1, 0], 4))
        slices.append(generalized_slice(multidim, [y + i, x + i], [1, 1], 4))
        slices.append(generalized_slice(multidim, [y - i, x + i], [-1, 1], 4))
    return slices

def color_of_winning_slice(multidim, x, y):
    slices = all_len4_slices(multidim, x, y)
    for color in range(Y, 0, -1):
        if [color] * 4 in slices:
            return color
    return 0

def get_win(multidim):
    for y in range(0, len(multidim)):
        x_len = len(multidim[y])
        for x in range(0, x_len):
            winner = color_of_winning_slice(multidim, x, y)
            print(winner, end = ", ")
        print()

get_win(board)
