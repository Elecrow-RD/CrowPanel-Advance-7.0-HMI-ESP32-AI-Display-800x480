import base64
import io
import json
import random
import time
import wave

import websockets
import asyncio
from asr import AsrWsClient
from config import CONNECTIONS, CLIENTS, TASKS
from llm import post_request
from llm_openai import chat

from db_redis import re, get_redis_config
from tts import test_submit


# Exception handling of asynchronous tasks
async def handle_exception(task, *args, **kwargs):
    try:
        await task(*args, **kwargs)
    except websockets.exceptions.ConnectionClosedError as e:
        print(f"(in)client {args[0]} Connection closed: {e}")
    except ConnectionResetError as e:
        print(f"(in)Connection reset error: {e}")
    except Exception as e:
        print(f"(in)An error occurred while processing audio: {e}")


# Conversational task
async def process_audio(mac_address):
    # The first step is to get the LLM+TTS configuration in the database or redis
    # get_config = await get_redis_config(mac_address) # Get personal redis dynamic configuration
    # Step 2 ASR
    asr_word = await AsrWsClient().send_full_request(mac_address)
    print(f"{mac_address}-ask:", asr_word)

    # step 3 LLM+TTS
    await chat(asr_word, mac_address)

    # Step 4 Update the redis configuration
    # await re.set(mac_address, json.dumps(per_config), ex=3600)  # Update chat history in redis


# Opening task
async def open_word(mac_address):
    get_config = await get_redis_config(mac_address)  # Get a personal redis dynamic configuration
    per_config = json.loads(get_config)
    tts_config = per_config['tts_config']
    sentence = random.choice([
       "OK"
    ])

    await test_submit(sentence, mac_address, tts_config)
    # await CLIENTS[mac_address].send("finish_tts")


# Wake up task
async def wake_up(mac_address):
    get_config = await get_redis_config(mac_address)  # Get a personal redis dynamic configuration
    per_config = json.loads(get_config)
    tts_config = per_config['tts_config']


    sentence = random.choice([
        "hello!"
    ])
    print("Wake up recovery:", sentence)
    await test_submit(sentence, mac_address, wake_up=True)
    await CLIENTS[mac_address].send("finish_tts")





async def handler(websocket, path):
    client_id = websocket.remote_address
    print(f"client {client_id} connected")
    try:
        async for message in websocket:
            # Received the json data sent by the client
            rcv_data = json.loads(message)
            event = rcv_data["event"]
            mac_address = rcv_data["mac_address"]
            data = rcv_data["data"]

            # Receive single-chip recording audio stream
            if event == "record_stream":
                audio_data = base64.b64decode(data)
                await CONNECTIONS[mac_address].put(audio_data)
            # restart process_audio
            elif event == "re_process_audio":
                print(mac_address,"start!")
                TASKS[mac_address] = asyncio.create_task(handle_exception(process_audio, mac_address))

            # prologue
            elif event == "open_word":
                CONNECTIONS[mac_address] = asyncio.Queue()
                CLIENTS[mac_address] = websocket
                print("prologue")
                TASKS[mac_address] = asyncio.create_task(handle_exception(process_audio, mac_address))


            # Wake up session
            elif event == "wake_up":
                print("Wake up session")
                CONNECTIONS[mac_address] = asyncio.Queue()
                CLIENTS[mac_address] = websocket
                TASKS[mac_address] = asyncio.create_task(handle_exception(wake_up, mac_address))



            # interrupt a conversation
            elif event == "interrupt_audio":
                print("interrupt a conversation")
                await cancel_process_audio_task(mac_address)
                time.sleep(2.2)
                TASKS[mac_address] = asyncio.create_task(handle_exception(process_audio, mac_address))


            # If no sound is transmitted for 10 seconds, the process_audio task is canceled
            elif event == "timeout_no_stream":
                print("If no sound is transmitted for 10 seconds, the process_audio task is canceled")
                await cancel_process_audio_task(mac_address)
                CLIENTS.pop(mac_address)
                CONNECTIONS.pop(mac_address)
                TASKS.pop(mac_address)



    except websockets.exceptions.ConnectionClosedError as e:
        print(f"(out)client {client_id} disconnect: {e}")

    except ConnectionResetError as e:
        print(f"(out)Connection reset error: {e}")

    except Exception as e:
        print(f"(out)An error occurred while processing the message: {e}")

    finally:
        # if mac_address in CONNECTIONS:
        #     del CONNECTIONS[mac_address]
        # if mac_address in CLIENTS:
        #     del CLIENTS[mac_address]
        print(f"client {client_id} Connection closed")




#cancel process_audio task
async def cancel_process_audio_task(mac_address):
    task = TASKS.get(mac_address)
    if task and not task.done():
        task.cancel()


async def main():
    async with websockets.serve(handler, "0.0.0.0", 8765):
        print("The server is started on port 8765, waiting for a connection...")
        await asyncio.Future()  # Permanent operation


if __name__ == '__main__':
    asyncio.run(main())
