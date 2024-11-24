#!/usr/bin/env python

from re import sub
from random import randint
from functools import reduce
from sys import argv

ziffern = ["rei", "ichi", "ni", "san", "yon", "go", "roku", "nana", "hachi", "kyuu"]
expos = ["", "juu", "hyaku", "sen"]
myriaden = ["", "man", "oku"]
ausnahmen = list(map(
	lambda r: lambda s: sub(r[0], r[1], s),
	[
		("sanhyaku", "sanbyaku"),
		("rokuhyaku", "roppyaku"),
		("hachihyaku", "happyaku"),
		("sansen", "sanzen"),
		("hachisen", "hassen"),
	]
))

def _chunkify(lst, n):
	for i in range(0, len(lst), n):
		yield lst[i : i + n]

def chunkify(lst, n):
	return list(_chunkify(lst, n))

parts = chunkify(chunkify(argv[1][::-1], 1), 4) # yeah this works trust me
rawString = " ".join(list(map(lambda part, myriade:
	" ".join(list(map(lambda d:
		ziffern[int(d[0])] + d[1], # digit name + exponent
		filter(lambda x: int(x[0]) > 0, zip(part, expos)) # attach all, then remove zeros
	))[::-1]) + " " + myriade, # reverse & join digits, then attach myriad
	parts, myriaden # we can use zipWith semantics here since the filter will be applied deeper
))[::-1]) # reverse & join parts
result = reduce(lambda s, f: f(s), ausnahmen, rawString) # apply all replacements
print(result)
