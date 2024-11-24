#!/usr/bin/env python3
(a, b, c, d, e) = '21686148-6449-6E6F-744E-656564454649'.split('-')

a = bytearray.fromhex(a)
a.reverse()
b = bytearray.fromhex(b)
b.reverse()
c = bytearray.fromhex(c)
c.reverse()
data = a + b + c + bytes.fromhex(d + e)
print(data.decode('utf8'))
