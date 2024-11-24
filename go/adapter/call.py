#!/usr/bin/env python3
import sys

if len(sys.argv) < 2:
    print("Usage: call <file>")
    exit(1)

with open(sys.argv[1], "r+b") as f:
    data = sys.stdin.buffer.read()
    f.write(data)
    f.seek(0)
    sys.stdout.buffer.write(f.read())
