#!/usr/bin/env python3
import sys
import asyncio
from websockets import connect

async def run():
    client = input("Client ID: ")
    sys.stdin.close()
    async with connect("wss://seashells.io/ws") as ws:
        await ws.send(client)
        while True:
            sys.stdout.buffer.write(await ws.recv())
            sys.stdout.buffer.flush()

asyncio.get_event_loop().run_until_complete(run())
