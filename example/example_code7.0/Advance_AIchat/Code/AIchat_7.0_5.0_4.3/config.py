# Used to store the recording audio stream queue of each MCU
CONNECTIONS={}
CLIENTS={}
TASKS={}

# ======================================= Database configuration ==========================================================

DATABASE_URL = "mysql+aiomysql://root:123456@localhost:3306/AI_TOY"



HISTORY = [
    {"role": "system",
     "content": "You're American. You only speak English."}
]






























# # Default parameters
# CONFIG_DICT = {
#     "bot_id": "7383896597031485490",  # Default child companion
#     "chat_history": [],
#     "voice_type": "BV064_streaming",  # Voice tone, default is a little girl's voice
#     "speed_ratio": 1.0,  # Speech speed, default is 1.0
#     "volume_ratio": 3.0,  # Volume, default is 3.0
#     "pitch_ratio": 1.0,  # Pitch, default is 1.0
# }

# # ======================================= COZE model configuration ========================================================
# COZE_HEADERS = {
#     'Content-Type': 'application/json; charset=utf-8',
#     'Authorization': 'Bearer pat_7hvslWGpZGLy3nqH4tUad4ZegGGyc7vNJXORrMxajws262S4QLjffirMIC3NNGYK',
#     'Connection': 'keep-alive',
#     'Accept': '*/*'
# }
# COZE_DATA = {
#     # 'bot_id': '7383896597031485490',  # Child companion
#     # 'bot_id': '7390379454117691402',  # Legend of Sword and Fairy
#     'bot_id': CONFIG_DICT['bot_id'],  # Dynamic bot_id
#     'chat_history': CONFIG_DICT['chat_history'],  # Dynamic chat_history
#     'user': 'Alvan',
#     'query': '',
#     'stream': False,
#     # 'conversation_id': self.conversation_id,
# }

# # ======================================= Speech synthesis configuration ==================================================
# TTS_CONFIG = {
#     "voice_type": CONFIG_DICT['voice_type'],
#     "speed_ratio": CONFIG_DICT['speed_ratio'],
#     "volume_ratio": CONFIG_DICT['volume_ratio'],
#     "pitch_ratio": CONFIG_DICT['pitch_ratio']
# }
