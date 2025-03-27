import aioredis
import json
from db import get_db
from model.model import MAiToy
from sqlalchemy import select

# Create an instance of redis
re = aioredis.from_url("redis://localhost:6379/5")


# Fetching dynamic parameters from the database
async def get_db_config(mac_addr, db):
    # Retrieves configuration parameters from the database
    obj = await db.execute(select(MAiToy).filter(MAiToy.mac_addr == mac_addr))
    obj=obj.scalars().first()

    # If there is no obj data, create a new one
    if not obj:
        new_obj = MAiToy(mac_addr=mac_addr)
        db.add(new_obj)
        await db.commit()
        obj = new_obj

    config_obj = {
        'bot_id': obj.bot_id,
        'voice_type': obj.voice_type,
        'speed_ratio': obj.speed_ratio,
        'volume_ratio': obj.volume_ratio,
        'pitch_ratio': obj.pitch_ratio
    }

    json_data = json.dumps({
        "coze_config": {
            "bot_id": config_obj['bot_id'],
            "chat_history": []
        },
        "tts_config": {
            "voice_type": config_obj['voice_type'],
            "speed_ratio": config_obj['speed_ratio'],
            "volume_ratio": config_obj['volume_ratio'],
            "pitch_ratio": config_obj['pitch_ratio']
        }
    })
    return json_data


# Get the redis configuration, if redis is configured, get the redis configuration, if redis is not configured, get the database configuration, and set the redis expiration time to 3600 seconds
async def get_redis_config(mac_addr):
    get_config = await re.get(mac_addr)
    # The redis dynamic parameter is obtained
    if not get_config:
        # Get database dynamic parameters
        db = await get_db()
        get_config = await get_db_config(mac_addr, db)
        # Example Set the redis expiration time to 3600 seconds
        await re.set(mac_addr, get_config, ex=3600)
    return get_config
