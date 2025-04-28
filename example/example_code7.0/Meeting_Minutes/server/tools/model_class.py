import websockets
import numpy as np
import soundfile as sf
import argparse
import os
import re
import json
import traceback
import time
from urllib.parse import parse_qs
from loguru import logger
import sys
from funasr import AutoModel
from modelscope.pipelines import pipeline
from modelscope.utils.constant import Tasks


# 配置参数 ---------------------------------------------------------------------
class AppConfig:
    def __init__(self):
        # Audio processing parameters
        self.sv_thr = 0.3  # Speaker verification threshold
        self.chunk_size_ms = 300  # Block size (milliseconds)
        self.sample_rate = 16000  # sampling frequency
        self.bit_depth = 16  # bit depth
        self.channels = 1  # AudioClip.channels
        self.avg_logprob_thr = -0.25  # Average logarithmic probability threshold


# Text format conversion class ---------------------------------------------------------------------
class StringFormatter:
    emo_dict = {
        "<|HAPPY|>": "😊",
        "<|SAD|>": "😔",
        "<|ANGRY|>": "😡",
        "<|NEUTRAL|>": "",
        "<|FEARFUL|>": "😰",
        "<|DISGUSTED|>": "🤢",
        "<|SURPRISED|>": "😮",
    }

    event_dict = {
        "<|BGM|>": "🎼",
        "<|Speech|>": "",
        "<|Applause|>": "👏",
        "<|Laughter|>": "😀",
        "<|Cry|>": "😭",
        "<|Sneeze|>": "🤧",
        "<|Breath|>": "",
        "<|Cough|>": "🤧",
    }

    emoji_dict = {
        "<|nospeech|><|Event_UNK|>": "❓",
        "<|zh|>": "",
        "<|en|>": "",
        "<|yue|>": "",
        "<|ja|>": "",
        "<|ko|>": "",
        "<|nospeech|>": "",
        "<|HAPPY|>": "😊",
        "<|SAD|>": "😔",
        "<|ANGRY|>": "😡",
        "<|NEUTRAL|>": "",
        "<|BGM|>": "🎼",
        "<|Speech|>": "",
        "<|Applause|>": "👏",
        "<|Laughter|>": "😀",
        "<|FEARFUL|>": "😰",
        "<|DISGUSTED|>": "🤢",
        "<|SURPRISED|>": "😮",
        "<|Cry|>": "😭",
        "<|EMO_UNKNOWN|>": "",
        "<|Sneeze|>": "🤧",
        "<|Breath|>": "",
        "<|Cough|>": "😷",
        "<|Sing|>": "",
        "<|Speech_Noise|>": "",
        "<|withitn|>": "",
        "<|woitn|>": "",
        "<|GBG|>": "",
        "<|Event_UNK|>": "",
    }

    lang_dict = {
        "<|zh|>": "<|lang|>",
        "<|en|>": "<|lang|>",
        "<|yue|>": "<|lang|>",
        "<|ja|>": "<|lang|>",
        "<|ko|>": "<|lang|>",
        "<|nospeech|>": "<|lang|>",
    }

    emo_set = {"😊", "😔", "😡", "😰", "🤢", "😮"}
    event_set = {"🎼", "👏", "😀", "😭", "🤧", "😷"}

    def format_str_v3(self, s):
        def get_emo(s_part):
            return s_part[-1] if s_part and s_part[-1] in self.emo_set else None

        def get_event(s_part):
            return s_part[0] if s_part and s_part[0] in self.event_set else None

        s = s.replace("<|nospeech|><|Event_UNK|>", "❓")
        # 统一语言标记
        for lang in self.lang_dict:
            s = s.replace(lang, self.lang_dict[lang])
        s_list = [self.format_str_v2(s_i.strip()) for s_i in s.split("<|lang|>")]

        if not s_list:
            return ""

        new_s = " " + s_list[0]
        cur_ent_event = get_event(s_list[0])

        for i in range(1, len(s_list)):
            if not s_list[i]:
                continue

            current_event = get_event(s_list[i])
            if current_event == cur_ent_event and current_event is not None:
                s_list[i] = s_list[i][1:]

            cur_ent_event = get_event(s_list[i]) if s_list[i] else None

            current_emo = get_emo(s_list[i])
            last_emo = get_emo(new_s)
            if current_emo and current_emo == last_emo:
                new_s = new_s[:-1].rstrip()

            new_s += " " + s_list[i].strip()

        new_s = new_s.replace("The.", " ").strip()
        return new_s

    def format_str_v2(self, s):
        sptk_dict = {}
        for sptk in self.emoji_dict:
            cnt = s.count(sptk)
            sptk_dict[sptk] = cnt
            s = s.replace(sptk, "")

        max_emo = "<|NEUTRAL|>"
        for emo in self.emo_dict:
            if sptk_dict.get(emo, 0) > sptk_dict.get(max_emo, 0):
                max_emo = emo

        for event in self.event_dict:
            if sptk_dict.get(event, 0) > 0:
                s = self.event_dict[event] + s

        s += self.emo_dict[max_emo]

        for emoji in self.emo_set.union(self.event_set):
            s = s.replace(f" {emoji}", emoji)
            s = s.replace(f"{emoji} ", emoji)

        return s.strip()



class ModelManager:
    def __init__(self, config: AppConfig):
        self.config = config
        self._init_models()

    def _init_models(self):
        """初始化所有模型"""
        self.sv_pipeline = pipeline(
            task='speaker-verification',
            model='iic/speech_campplus_sv_zh-cn_16k-common',
            model_revision='v1.0.0'
        )

        self.asr_model = AutoModel(
            model="iic/SenseVoiceSmall",
            trust_remote_code=True,
            remote_code="./model.py",
            device="cuda:0",
            disable_update=True
        )

        self.vad_model = AutoModel(
            model="iic/speech_fsmn_vad_zh-cn-16k-common-pytorch",
            model_revision="v2.0.4",
            disable_pbar=True,
            max_end_silence_time=500,
            disable_update=True,
        )

        self.registered_speakers = {}



def build_response(code=0, info="", data="") -> dict:
    """构建响应字典"""
    return {
        "code": code,
        "info": info,
        "data": data
    }


def build_error_response(code=500, message="Internal server error", data=""):
    """Construct an error response"""
    return build_response(
        code=code,
        info=message,
        data=data
    )


