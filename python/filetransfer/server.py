import socket
from threading import Thread, get_ident
from socketserver import ThreadingMixIn

class ClientThread(Thread):
    def __init__(self, conn):
        Thread.__init__(self)
        self.sock = conn[0]
        self.ip   = conn[1][0]
        self.port = conn[1][1]

    def run(self):
        id = get_ident()
        print(f"{id}: Started for {self.ip}:{self.port}")
        try:
            # receive packet
            while True:
                data = b""
                print(f"{id}: Ready")

                # receive header
                while True:
                    byte = self.sock.recv(1)
                    if not byte:
                        raise Exception("Incomplete header")
                    if byte == b"\0":
                        # header ends with nullbyte
                        break
                    # byte is part of header
                    data += byte

                # get length
                length = int.from_bytes(data, "big")
                if length < 1:
                    print(f"{id}: Connection closed")
                    break
                data = b""

                # receive data
                received = 0
                print(f"{id}: Expecting {length} bytes")
                while received < length:
                    buffer = self.sock.recv(length - received)
                    if not buffer:
                        raise Exception("Incomplete data")
                    data += buffer
                    received += len(buffer)

                # print data
                print(f"Data: {data}")
                print(data.decode("utf-8"))
        except Exception as e:
            print(f"\nException caught in {id}: {e}")
        finally:
            print(f"{id}: Exiting")
            self.sock.close()

TCP_IP = '0.0.0.0'
TCP_PORT = 37812
BUFFER_SIZE = 1024

tcpServer = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
tcpServer.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
tcpServer.bind((TCP_IP, TCP_PORT))
threads = []

while True:
    tcpServer.listen(4)
    print("Server: waiting for new connection")
    newthread = ClientThread(tcpServer.accept())
    newthread.start()
    threads.append(newthread)
