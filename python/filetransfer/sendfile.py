import socket
import sys
from   math import log, floor

content = sys.stdin.read().encode("utf-8")
header_length = floor(log(len(content), 256)) + 1
header = len(content).to_bytes(header_length, "big")
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.connect(("42loco42.duckdns.org", 37812))
    sock.sendall(header)
    sock.sendall(b"\0")
    sock.sendall(content)
    sock.sendall(b"\0")
