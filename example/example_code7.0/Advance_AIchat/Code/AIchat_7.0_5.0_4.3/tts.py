# coding=utf-8

'''
requires Python 3.6 or later

pip install asyncio
pip install websockets

'''

import asyncio
import threading
import time
from io import BytesIO
import websockets
import uuid
import json
import gzip
import copy
from config import CLIENTS

MESSAGE_TYPES = {11: "audio-only server response", 12: "frontend server response", 15: "error message from server"}
MESSAGE_TYPE_SPECIFIC_FLAGS = {0: "no sequence number", 1: "sequence number > 0",
                               2: "last message from server (seq < 0)", 3: "sequence number < 0"}
MESSAGE_SERIALIZATION_METHODS = {0: "no serialization", 1: "JSON", 15: "custom type"}
MESSAGE_COMPRESSIONS = {0: "no compression", 1: "gzip", 15: "custom compression method"}

appid = "YOUR-ID"
token = "YOUR-TOKEN"
cluster = "volcano_tts"

voice_type = "BV700_streaming"
host = "openspeech.bytedance.com"
api_url = f"wss://{host}/api/v1/tts/ws_binary"

# version: b0001 (4 bits)
# header size: b0001 (4 bits)
# message type: b0001 (Full client request) (4bits)
# message type specific flags: b0000 (none) (4bits)
# message serialization method: b0001 (JSON) (4 bits)
# message compression: b0001 (gzip) (4bits)
# reserved data: 0x00 (1 byte)
default_header = bytearray(b'\x11\x10\x11\x00')


async def test_submit(text, mac_address, wake_up=False):
    uid = str(uuid.uuid4())

    request_json = {
        "app": {
            "appid": appid,
            "token": "access_token",
            "cluster": cluster
        },
        "user": {
            "uid": "YOUR-USERID"
        },
        "audio": {
            "voice_type": 'BV064_streaming',
            "encoding": "pcm",
            "speed_ratio":  1.0,
            "volume_ratio": 2.0,
            "pitch_ratio": 1.0,
            # "language": "en"
        },
        "request": {
            "reqid": uid,
            "text": text,
            "text_type": "plain",
            "operation": "submit"
        }
    }
    payload_bytes = str.encode(json.dumps(request_json))
    payload_bytes = gzip.compress(payload_bytes)  # if no compression, comment this line
    full_client_request = bytearray(default_header)
    full_client_request.extend((len(payload_bytes)).to_bytes(4, 'big'))  # payload size(4 bytes)
    full_client_request.extend(payload_bytes)  # payload
    header = {"Authorization": f"Bearer; {token}"}
    async with websockets.connect(api_url, extra_headers=header, ping_interval=None) as ws:
        await ws.send(full_client_request)
        while True:
            res = await ws.recv()
            done = await parse_response(res, mac_address,wake_up)
            if done:
                break


async def parse_response(res, mac_address,wake_up):
    protocol_version = res[0] >> 4
    header_size = res[0] & 0x0f
    message_type = res[1] >> 4
    message_type_specific_flags = res[1] & 0x0f
    serialization_method = res[2] >> 4
    message_compression = res[2] & 0x0f
    reserved = res[3]
    header_extensions = res[4:header_size * 4]
    payload = res[header_size * 4:]
    if header_size != 1:
        print(f"           Header extensions: {header_extensions}")
    if message_type == 0xb:  # audio-only server response
        if message_type_specific_flags == 0:  # no sequence number as ACK
            return False
        else:
            sequence_number = int.from_bytes(payload[:4], "big", signed=True)
            # payload_size = int.from_bytes(payload[4:8], "big", signed=False)
            payload = payload[8:]

        audio_data = BytesIO(payload)
        # 8000 bytes are sent each time
        while True:
            chunk = audio_data.read(8000)
            if chunk:
                # if wake_up:
                #     chunk=b'\x01'+chunk
                await CLIENTS[mac_address].send(chunk)
            else:
                break

        if sequence_number < 0:
            return True
        else:
            return False
    elif message_type == 0xf:
        code = int.from_bytes(payload[:4], "big", signed=False)
        msg_size = int.from_bytes(payload[4:8], "big", signed=False)
        error_msg = payload[8:]
        if message_compression == 1:
            error_msg = gzip.decompress(error_msg)
        error_msg = str(error_msg, "utf-8")
        # print(f"          Error message code: {code}")
        # print(f"          Error message size: {msg_size} bytes")
        # print(f"               Error message: {error_msg}")
        return True
    elif message_type == 0xc:
        msg_size = int.from_bytes(payload[:4], "big", signed=False)
        payload = payload[4:]
        if message_compression == 1:
            payload = gzip.decompress(payload)
        # print(f"            Frontend message: {payload}")
    else:
        # print("undefined message type!")
        return True


if __name__ == '__main__':
    loop = asyncio.get_event_loop()
    loop.run_until_complete(test_submit())
