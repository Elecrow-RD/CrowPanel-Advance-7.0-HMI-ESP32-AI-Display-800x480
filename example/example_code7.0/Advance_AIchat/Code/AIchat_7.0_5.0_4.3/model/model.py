from sqlalchemy import Column, Integer, String, Float
from sqlalchemy.ext.declarative import declarative_base

Base = declarative_base()

class MAiToy(Base):
    __tablename__ = "m_ai_toy"
    __table_args__ = {'comment': '玩具AI数据表'}

    id = Column(Integer, primary_key=True, autoincrement=True, comment="主键")
    mac_addr = Column(String(15), unique=True, nullable=False, comment="MAC地址")
    bot_id = Column(String(25), default="7428427454739447845", comment="机器人ID")
    voice_type = Column(String(30), default="BV064_streaming", comment="声音类型")
    speed_ratio = Column(Float, default=1.0, comment="速度比率")
    volume_ratio = Column(Float, default=2.0, comment="音量比率")
    pitch_ratio = Column(Float, default=1.0, comment="音调比率")

