import asyncio
import websockets

async def listener(websocket):
    while True:
        await asyncio.sleep(0)
        try:
            msg = await websocket.recv()
            print(f"< {msg}")
        except Exception as e:
            print(e)
            break

async def producer(websocket):
    try:
        await asyncio.sleep(2)
        await websocket.send("foo")
    except Exception as e:
        print(e)

async def hello(uri):
    async with websockets.connect(uri) as websocket:
        await websocket.send("Hello world!")
        await asyncio.gather(
            listener(websocket),
            producer(websocket)
        )

asyncio.get_event_loop().run_until_complete(
    hello('ws://localhost:37812')
)
