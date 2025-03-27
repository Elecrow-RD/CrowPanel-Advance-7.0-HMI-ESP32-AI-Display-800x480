from openai import OpenAI

from config import CLIENTS, HISTORY

from llm import test_submit
from openai import AsyncOpenAI

# ___________This is deepseekinterface________________________________________________________________________
#client = AsyncOpenAI(
#    api_key="YOUR-KEY",
    # Replace MOONSHOT_API_KEY here with the API Key you applied for from the OpenAI platform
#    base_url="https://api.deepseek.com"  # Replace the URL address
#)
#model="deepseek-chat"



# ____________This is the openAI interface_______________________________________________________________________
client = AsyncOpenAI(
     api_key="YOUR-KEY",
     # Here replace MOONSHOT_API_KEY with the API Key you requested from the Openai open platform
     base_url="https://api.openai.com/v1", #Alternate url address
 )
model="gpt-4o"


# ____________This is the zero one everything interface_______________________________________________________________________
# client = AsyncOpenAI(
#     api_key="YOUR-KEY",
#     # Replace MOONSHOT_API_KEY here with the API Key you applied for from the OpenAI platform
#     base_url="https://api.lingyiwanwu.com/v1"  # Replace the URL address
# )
# model = "yi-lightning"


async def chat(input, mac_address, sentence="", all_sentence=""):
    # global HISTORY
    # We construct the user's latest question into a message (role=user) and add it to the end of the messages list.

    HISTORY.append({
        "role": "user",
        "content": input,
    })

    # Have a conversation with the OpenAI large model using the messages.
    completion = await client.chat.completions.create(
        model=model,
        messages=HISTORY,
        temperature=0.3,
        stream=True
    )

    # Through the API, we get the reply message (role=assistant) from the OpenAI large model.
    count = 1

    async for chunk in completion:

        choices_obj = chunk.choices[0]
        content = choices_obj.delta.content
        # print(chunk)
        finish_reason = choices_obj.finish_reason

        if not finish_reason:
            if not content:
                continue
            all_sentence += content
            sentence += content
            if content in ['。', '！', '？', '，', '.', '!', '?', ','] and len(sentence) >= 6 ** count:
                # Run TTS in pure streaming mode.
                # print(f"Sentence {str(count)}: {sentence}")
                # print(sentence)
                # await CLIENTS[mac_address].send(sentence)

                await test_submit(sentence, mac_address)
                count += 1
                sentence = ""

        else:
            await test_submit(sentence, mac_address)
            sentence = ""
            print("total:", all_sentence)
            # await CLIENTS[mac_address].send("clear_screen")
            await CLIENTS[mac_address].send("finish_tts")
    # To enable the OpenAI large model to have a complete memory, we must also add the message returned by the OpenAI large model to the HISTORY list.
    HISTORY.append({
        "role": "assistant",
        "content": all_sentence
    })

    # return assistant_message.content
