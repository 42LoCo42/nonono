#!/usr/bin/env python3
import dns.resolver
import requests
import urllib.parse

from http.server import SimpleHTTPRequestHandler
from socketserver import TCPServer

HOST = "www.anisopteragames.com"

res = dns.resolver.Resolver()
answer = res.resolve(HOST, "a")
url = f"http://{answer[0]}"
print(url)


class Handler(SimpleHTTPRequestHandler):
    def proxy(self, method):
        params = urllib.parse.parse_qs(urllib.parse.urlparse(self.path).query)

        if params.get("op") == ["upload_agent"]:
            print("BLOCKING UPLOAD!")
            self.send_response(200)
            self.send_header("connection", "close")
            self.end_headers()
            self.wfile.write("OK".encode("utf-8"))
            return

        r = method(
            f"{url}{self.path}",
            headers={"Host": HOST},
        )

        self.send_response(r.status_code)
        r.headers["content-length"] = len(r.content)
        del r.headers["connection"]
        del r.headers["keep-alive"]
        del r.headers["upgrade"]

        for header, value in r.headers.items():
            self.send_header(header, value)
        self.end_headers()

        self.wfile.write(r.content)

    def do_GET(self):
        self.proxy(requests.get)

    def do_POST(self):
        self.proxy(requests.post)


TCPServer.allow_reuse_address = True
server = TCPServer(("localhost", 80), Handler)
server.serve_forever()
