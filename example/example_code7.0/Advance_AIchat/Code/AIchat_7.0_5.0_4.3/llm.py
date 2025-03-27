import asyncio
import json
import aiohttp
from tts import test_submit
from config import CLIENTS

url = 'https://api.coze.cn/open_api/v2/chat'
COZE_HEADERS = {
    'Content-Type': 'application/json; charset=utf-8',
    'Authorization': 'Bearer YOUR-KEY',
    'Connection': 'keep-alive',
    'Accept': '*/*'
}
COZE_DATA = {
    'bot_id': 'YOUR-ID',  # Dynamic bot_id child companionship
    'chat_history': [],  # Dynamic chat history
    'user': 'Alvan',
    'query': '',
    'stream': True,
}



async def post_request(query,mac_address,get_config,sentence="", all_sentence=""):
    per_config = json.loads(get_config)  # Convert to dictionary
    # Overwrites the coze configuration
    coze_config = per_config['coze_config']
    # print("Button arrangement:",coze_config)
    COZE_DATA['bot_id'] = coze_config['bot_id']
    COZE_DATA['chat_history']=coze_config['chat_history']
    COZE_DATA['query'] = query
    histories=COZE_DATA['chat_history']
    histories.append({
        'role': 'user',
        'content': query,
        'content_type': 'text'
    })


    tts_config = per_config['tts_config']
    # print("TTS configration:",tts_config)

    async with aiohttp.ClientSession() as session:
        async with session.post(url, headers=COZE_HEADERS, json=COZE_DATA) as response:
            if response.status == 200:
                # Read SSE stream
                count = 1
                async for line in response.content:
                    data = line.decode('utf-8').strip()[5:]
                    if data:
                        # Decode each line
                        json_data = json.loads(data)
                        event = json_data['event']
                        if event !='done':
                            is_finish=json_data['is_finish']
                        if event == 'message':
                            message = json_data['message']
                            content = message['content']
                            all_sentence += content.split('{')[0]
                            sentence += content.split('{')[0]


                            if content in ['。', '！', '？', '，','.','!','?',','] and len(sentence) >= 6 ** count:
                                # run tts Pure flow
                                # print(f"The{str(count)}line:",sentence)
                                await test_submit(sentence,mac_address,tts_config)
                                count += 1
                                sentence = ""
                            elif content == '' and len(sentence) < 6 ** count and is_finish and json_data['index']==0:
                                # print(f"Last sentence:",sentence)
                                if sentence!='':
                                    await test_submit(sentence,mac_address,tts_config)
                                    sentence = ""

                        elif event == 'done':
                            pass
                            
                        elif event == 'error':
                            error_information = json_data['error_information']
                            print(
                                f"chat End of error, error code：{error_information['code']}，error message：{error_information['msg']}")
    await CLIENTS[mac_address].send("finish_tts")
    # Response to the return of the record
    assistant_res={
        "role": "assistant",
        "type": "answer",
        "content": all_sentence,
        "content_type": "text"
    }
    histories.append(assistant_res)
    if len(histories)>=20:
        del histories[:2]
    print(f"{mac_address}-answer:",all_sentence)
    return per_config










# Run asynchronous function
if __name__ == '__main__':
    asyncio.run(post_request())
