import asyncio
import websockets

async def ping5(websocket):
    for i in range(5):
        await websocket.send(f"{i}")

async def echo(websocket, path):
    while True:
        try:
            msg = await websocket.recv()
            print(f"< {msg}")
            await asyncio.gather(
                websocket.send(msg),
                ping5(websocket)
            )
        except Exception as e:
            print(e)
            break

asyncio.get_event_loop().run_until_complete(
    websockets.serve(echo, "localhost", 37812)
)
asyncio.get_event_loop().run_forever()
