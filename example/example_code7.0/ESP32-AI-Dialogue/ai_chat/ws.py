import base64
import json
import random
import time

import websockets
import asyncio
from tools.asr import AsrWsClient
from tools.config import CONNECTIONS, CLIENTS, TASKS
from tools.llm_openai import chat

from tools.tts import test_submit


# Exception handling for asynchronous tasks
async def handle_exception(task, *args, **kwargs):
    try:
        await task(*args, **kwargs)
    except websockets.exceptions.ConnectionClosedError as e:
        print(f"(in)Client {args[0]} connection closed: {e}")
    except ConnectionResetError as e:
        print(f"(in)Connection reset error: {e}")
    except Exception as e:
        print(f"(in)Error processing audio: {e}")


# Conversation task
async def process_audio(mac_address):
    # Step 1: Get LLM+TTS configuration from database or Redis
    # get_config = await get_redis_config(mac_address)  # Retrieve dynamic configuration from Redis
    # Step 2: ASR
    asr_word = await AsrWsClient().send_full_request(mac_address)
    await CLIENTS[mac_address].send(f"USER:{asr_word}")
    print(f"{mac_address}-Question:", asr_word)

    # Step 3: LLM+TTS
    await chat(asr_word, mac_address)

    # Step 4: Update Redis configuration
    # await re.set(mac_address, json.dumps(per_config), ex=3600)  # Update chat history in Redis


# Greeting task
async def open_word(mac_address):

    sentence = random.choice([
       "Okay"
    ])

    await test_submit(sentence, mac_address)
    # await CLIENTS[mac_address].send("finish_tts")


# Wake-up task
async def wake_up(mac_address):
    sentence = random.choice([
        "hello!"
    ])
    print("Wake-up response:", sentence)
    await test_submit(sentence, mac_address, wake_up=True)
    await CLIENTS[mac_address].send("finish_tts")




async def handler(websocket, path):
    client_id = websocket.remote_address
    print(f"Client {client_id} connected")
    try:
        async for message in websocket:
            # Receive JSON data from client
            rcv_data = json.loads(message)
            event = rcv_data["event"]
            mac_address = rcv_data["mac_address"]
            data = rcv_data["data"]

            # Receive audio stream from MCU
            if event == "record_stream":
                audio_data = base64.b64decode(data)
                await CONNECTIONS[mac_address].put(audio_data)
            # Restart process_audio
            elif event == "re_process_audio":
                print(mac_address,"Started!")
                TASKS[mac_address] = asyncio.create_task(handle_exception(process_audio, mac_address))

            # Greeting
            elif event == "open_word":
                CONNECTIONS[mac_address] = asyncio.Queue()
                CLIENTS[mac_address] = websocket
                print("Greeting")
                TASKS[mac_address] = asyncio.create_task(handle_exception(process_audio, mac_address))


            # Wake-up conversation
            elif event == "wake_up":
                print("Wake-up conversation")
                CONNECTIONS[mac_address] = asyncio.Queue()
                CLIENTS[mac_address] = websocket
                TASKS[mac_address] = asyncio.create_task(handle_exception(wake_up, mac_address))



            # Interrupt conversation
            elif event == "interrupt_audio":
                print("Interrupting conversation")
                await cancel_process_audio_task(mac_address)
                time.sleep(2.2)
                TASKS[mac_address] = asyncio.create_task(handle_exception(process_audio, mac_address))


            # Cancel process_audio if no sound is transmitted for 10 seconds
            elif event == "timeout_no_stream":
                print("No sound transmitted in 10 seconds, canceling process_audio task")
                await cancel_process_audio_task(mac_address)
                CLIENTS.pop(mac_address)
                CONNECTIONS.pop(mac_address)
                TASKS.pop(mac_address)



    except websockets.exceptions.ConnectionClosedError as e:
        print(f"(out)Client {client_id} disconnected: {e}")

    except ConnectionResetError as e:
        print(f"(out)Connection reset error: {e}")

    except Exception as e:
        print(f"(out)Error handling message: {e}")

    finally:
        print(f"Client {client_id} connection closed")




# Cancel process_audio task
async def cancel_process_audio_task(mac_address):
    task = TASKS.get(mac_address)
    if task and not task.done():
        task.cancel()


async def main():
    async with websockets.serve(handler, "0.0.0.0", 8765):
        print("Server started on port 8765, waiting for connections...")
        await asyncio.Future()  # Run indefinitely


if __name__ == '__main__':
    asyncio.run(main())