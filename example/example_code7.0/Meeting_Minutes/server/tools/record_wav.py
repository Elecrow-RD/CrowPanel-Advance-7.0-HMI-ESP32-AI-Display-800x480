import wave

from .config import RECORDING



async def record(name):
    print(f"开始录音...")
    with wave.open(f'speakers/{name}.wav', 'wb') as wf:
        wf.setnchannels(1) 
        wf.setsampwidth(2) 
        wf.setframerate(16000)  

        while True:
            audio_chunk = await RECORDING.get()
            if audio_chunk==b'\xd3\x1d8':
                break
            wf.writeframes(audio_chunk)
    return
