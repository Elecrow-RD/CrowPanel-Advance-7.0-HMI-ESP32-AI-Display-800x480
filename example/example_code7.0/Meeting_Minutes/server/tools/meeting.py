import os
import time

from .config import MEETING_STREAM
from . import config
from .model_class import AppConfig,StringFormatter,ModelManager
import numpy as np
from loguru import logger
import soundfile as sf



async def start():
    # Initialize configuration, tools, models
    config1 = AppConfig()
    global model_mgr
    model_mgr = ModelManager(config1)
    parse_string=StringFormatter()

    chunk_size = int(config1.chunk_size_ms * config1.sample_rate / 1000)
    audio_buffer = np.array([], dtype=np.float32)
    audio_vad = np.array([], dtype=np.float32)

    cache = {}
    cache_asr = {}
    last_vad_beg = last_vad_end = -1
    offset = 0
    hit = False

    buffer = b""
    speaker = None
    sv=True  #It indicates that the speaker needs to be identified
    lang="en"  #Multi-Language configuration

    while True:

        data = await MEETING_STREAM.get()

        buffer += data
        if len(buffer) < 2:
            continue

        audio_buffer = np.append(
            audio_buffer,
            np.frombuffer(buffer[:len(buffer) - (len(buffer) % 2)], dtype=np.int16).astype(np.float32) / 32767.0
        )



        buffer = buffer[len(buffer) - (len(buffer) % 2):]

        while len(audio_buffer) >= chunk_size:
            chunk = audio_buffer[:chunk_size]
            audio_buffer = audio_buffer[chunk_size:]
            audio_vad = np.append(audio_vad, chunk)

            if last_vad_beg > 1:
                if sv:
                    # speaker verify
                    # If no hit is detected, continue accumulating audio data and check again until a hit is detected
                    # `hit` will reset after `asr`.
                    if not hit:
                        hit, speaker = speaker_verify(
                            audio_vad[int((last_vad_beg - offset) * config1.sample_rate / 1000):], config1.sv_thr)
                        if hit:
                            pass
                else:
                    pass
            res = model_mgr.vad_model.generate(input=chunk, cache=cache, is_final=False, chunk_size=config1.chunk_size_ms)

            if len(res[0]["value"]):
                vad_segments = res[0]["value"]
                for segment in vad_segments:
                    if segment[0] > -1:  # speech begin
                        last_vad_beg = segment[0]
                    if segment[1] > -1:  # speech end
                        last_vad_end = segment[1]
                    if last_vad_beg > -1 and last_vad_end > -1:
                        last_vad_beg -= offset
                        last_vad_end -= offset
                        offset += last_vad_end
                        beg = int(last_vad_beg * config1.sample_rate / 1000)
                        end = int(last_vad_end * config1.sample_rate / 1000)
                        logger.info(f"[vad segment] audio_len: {end - beg}")
                        result = None if sv and not hit else asr(audio_vad[beg:end], lang.strip(), cache_asr, True)
                        logger.info(f"asr response: {result}")
                        audio_vad = audio_vad[end:]
                        last_vad_beg = last_vad_end = -1
                        hit = False
                        if result is not None:
                            speaker_data = speaker + ":" + result[0]['text'].split("<|withitn|>")[1] if speaker else result[0]['text']
                            # res=parse_string.format_str_v3(speaker_data)
                            print(speaker_data)
                            config.MEETING_LOG+=speaker_data+"\n"





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




def speaker_verify(audio, sv_thr):
    h=""
    hit = False
    for k, v in config.reg_spks.items():
        res_sv = model_mgr.sv_pipeline([audio, v["data"]], sv_thr)
        if res_sv["score"] >= sv_thr:
            hit = True
        # if res_sv["text"]=="yes":
            h=k
        logger.info(f"[speaker_verify] audio_len: {len(audio)}; sv_thr: {sv_thr}; hit: {hit}; {k}: {res_sv}")
    return hit,h


def asr(audio, lang, cache, use_itn=False):
    start_time = time.time()
    result = model_mgr.asr_model.generate(
        input=audio,
        cache=cache,
        language=lang.strip(),
        use_itn=use_itn,
        batch_size_s=60,
    )
    end_time = time.time()
    elapsed_time = end_time - start_time
    logger.debug(f"asr elapsed: {elapsed_time * 1000:.2f} milliseconds")
    return result