import asyncio

from openai import OpenAI

from .config import CLIENTS, HISTORY
from openai import AsyncOpenAI
from .tts import test_submit

# ___________This is the Deepseek interface________________________________________________________________________
# client = AsyncOpenAI(
#     api_key="sk-proj-your-api-key",
#     # Replace MOONSHOT_API_KEY here with the API Key you applied for from the OpenAI platform
#     base_url="https://api.deepseek.com"  # Replace the URL address
# )
# model="deepseek-chat"


# ____________This is the Kimi interface_______________________________________________________________________
# client = AsyncOpenAI(
#     api_key="sk-proj-your-api-key",
#     # Replace MOONSHOT_API_KEY here with the API Key you applied for from the OpenAI platform
#     base_url= "https://api.moonshot.cn/v1" # Replace the URL address
# )
# model="moonshot-v1-8k"


# ____________This is the Lingyiwanwu interface_______________________________________________________________________
# client = AsyncOpenAI(
#     api_key="sk-proj-your-api-key",
#     # Replace MOONSHOT_API_KEY here with the API Key you applied for from the OpenAI platform
#     base_url="https://api.lingyiwanwu.com/v1"  # Replace the URL address
# )
# model = "yi-lightning"


# ____________This is the OpenAI interface_______________________________________________________________________
client = AsyncOpenAI(
    api_key="sk-proj-your-api-key",
    base_url="https://api.openai.com/v1"
)
model = "gpt-4"



async def chat(input, mac_address, sentence="", all_sentence=""):
    # We construct the user's latest question into a message (role=user) and add it to the end of the messages list.

    HISTORY.append({
        "role": "user",
        "content": input,
    })

    # Send the chat history to the OpenAI model
    completion = await client.chat.completions.create(
        model=model,
        messages=HISTORY,
        temperature=0.3,
        stream=True
    )

    # Process the response from the OpenAI model
    count = 1

    async for chunk in completion:

        choices_obj = chunk.choices[0]
        content = choices_obj.delta.content
        finish_reason = choices_obj.finish_reason

        if not finish_reason:
            if not content:
                continue
            all_sentence += content
            sentence += content


            def split_sentence(s, max_len):
                for i in range(min(max_len, len(s)), 0, -1):
                    if s[i - 1] in ['。', '！', '？', '，', '.', '!', '?', ',']:
                        return s[:i], s[i:]
                return s[:max_len], s[max_len:]

            while len(sentence) > 100:
                part, sentence = split_sentence(sentence, 100)
                print("Looping output:", sentence)
                part = part.replace("/n", "")
                await CLIENTS[mac_address].send(f"AI:{part}")
                await test_submit(part, mac_address)
                count += 1


            if content in ['。', '！', '？', '，', '.', '!', '?', ','] and (len(sentence) >= 6 ** count or len(sentence) >= 100):
                # Streaming TTS
                print("Sending response:", sentence)
                sentence = sentence.replace("/n", "")

                await CLIENTS[mac_address].send(f"AI:{sentence}")
                await test_submit(sentence, mac_address)
                count += 1
                sentence = ""

        else:
            print("Final response to send:", sentence)
            sentence = sentence.replace("/n", "")

            await CLIENTS[mac_address].send(f"AI:{sentence}")
            await test_submit(sentence, mac_address)
            sentence = ""
            print("Complete response:", all_sentence)
            await CLIENTS[mac_address].send("finish_tts")

    # Add the response to HISTORY for continuity
    HISTORY.append({
        "role": "assistant",
        "content": all_sentence
    })
