import asyncio

from openai import AsyncOpenAI
from tools import config

client = AsyncOpenAI(
    api_key="YOUR-KEY",
    base_url="https://api.openai.com/v1"
)

model = "gpt-4"


async def gen_meeting_content():
    prompt = f"""
You are a meeting record summarization assistant. Please extract the meeting minutes from the following chat record, including the following aspects:
Meeting theme
Main discussion content
Decision - making items
Follow - up action plans
Please present the output in a clear and well - structured format.

Here is the chat record：
{config.MEETING_LOG}
"""
    response = await client.chat.completions.create(
        model=model,
        messages=[
            {"role": "system", "content": "You are an assistant for summarizing meeting content."},
            {"role": "user", "content": prompt}
        ],
        temperature=0.5
    )

    summary = response.choices[0].message.content
    with open("meeting_content.txt", "w", encoding="utf-8") as f:
        f.write(summary)

    print(f"The meeting minutes have been saved to：meeting_content.txt")
    return summary



async def gen_mindmap_md(summary: str):
    prompt = f"""
Please generate a Markdown format for the XMind mind map structure based on the following meeting minutes. Do not add any additional explanations or clarifications, and only output the standard Markdown content. Use the first - level heading # to represent the theme name, and use ## and - for hierarchical progression of branch content.
Here are the meeting minutes:
{summary}
"""

    response = await client.chat.completions.create(
        model=model,
        messages=[
            {"role": "system", "content": "You are an XMind mind map generator."},
            {"role": "user", "content": prompt}
        ],
        temperature=0.5
    )

    mindmap_md = response.choices[0].message.content

    with open("meeting_mindmap.md", "w", encoding="utf-8") as f:
        f.write(mindmap_md)

    print("Mind maps have been saved to：meeting_mindmap.md")



