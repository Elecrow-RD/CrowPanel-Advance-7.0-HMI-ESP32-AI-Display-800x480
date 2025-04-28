import asyncio
import base64
import json
import os
import shutil

from tools.config import RECORDING,MEETING_STREAM
from tools import config
import websockets
from tools.record_wav import record
from tools.meeting import start
from tools.open_ai import gen_meeting_content,gen_mindmap_md
import soundfile as sf


meeting_task = None 

async def event_handler(websocket, path):


    """Main message processor"""
    client_id = websocket.remote_address
    print(f"client {client_id} connected")

    try:
        async for message in websocket:
            rcv_data = json.loads(message)
            event = rcv_data["event"]
            data=rcv_data["data"]

            # print(rcv_data)

            if event == "record":
                await asyncio.sleep(0.2)
                asyncio.create_task(handle_exception(record_wav,data))
                pass
            elif event=="recording_stream":
                audio_data = base64.b64decode(data)
                await RECORDING.put(audio_data)

            elif event=="start_meeting":
                print(f"get the meeting started")
                folder_path="speakers"
                wav_files = [folder_path+"/"+f for f in os.listdir(folder_path) if
                             f.endswith('.wav') and os.path.isfile(os.path.join(folder_path, f))]
                config.reg_spks = reg_spk_init(wav_files)
                # Start the kick-off meeting task
                global meeting_task
                meeting_task = asyncio.create_task(handle_exception(start_meeting))

            elif event=="meeting_stream":
                audio_data = base64.b64decode(data)
                await MEETING_STREAM.put(audio_data)

            elif event=="end_meeting":
                print("meeting adjourned")
                if meeting_task and not meeting_task.done():
                    meeting_task.cancel()
                    try:
                        await meeting_task  
                    except asyncio.CancelledError:
                        print("The meeting task has been cancelled")
                meeting_task = None
                with open("meeting_log.txt", 'w', encoding='utf-8') as file:
                    file.write(config.MEETING_LOG)
                print("The meeting minutes have been saved as meeting_log.txt")

                summary=await gen_meeting_content()

                await gen_mindmap_md(summary)


                for filename in os.listdir("speakers"):
                    os.remove("speakers/"+filename)


    except websockets.ConnectionClosed:
        print(f"client {client_id} disconnected")
    finally:
        pass



async def record_wav(name):
    await record(name)
    print(f"The audio data has been saved as {name}.wav")
    folder_path = "speakers"
    wav_files = [folder_path + "/" + f for f in os.listdir(folder_path) if
                 f.endswith('.wav') and os.path.isfile(os.path.join(folder_path, f))]
    config.reg_spks = reg_spk_init(wav_files)


async def start_meeting():
    await start()


def reg_spk_init(files):
    reg_spk = {}
    for f in files:
        data, sr = sf.read(f, dtype="float32")
        k, _ = os.path.splitext(os.path.basename(f))
        reg_spk[k] = {
            "data": data,
            "sr": sr,
        }
    return reg_spk




async def handle_exception(task, *args, **kwargs):
    try:
        await task(*args, **kwargs)
    except websockets.exceptions.ConnectionClosedError as e:
        print(f"(in) client {args[0]} connection has been closed: {e}")
    except ConnectionResetError as e:
        print(f"(in)Connection reset error: {e}")
    except Exception as e:
        print(f"(in)error occurred when processing the audio: {e}")


async def main():
    """start WebSocket server"""
    async with websockets.serve(event_handler, "0.0.0.0", 27000):
        print("WebSocket server has been started on port 27000")
        await asyncio.Future() 


if __name__ == "__main__":
    asyncio.run(main())