#pragma once

#define LOG_DEBUG() do{           \
  Serial.print("FUNCTION[");      \
  Serial.print(__FUNCTION__);     \
  Serial.print("]\tLINE[");       \
  Serial.print(__LINE__);         \
  Serial.println("]");            \
}while(0)

#define ADDRESS  0x18
#define INPUT_REG     0x00
#define OUTPUT_REG    0x01
#define POLARITY_REG  0x02
#define CONFIG_REG    0x03

// 将指定的bit位置1
#define BIT_SET(pin_val, pin) ((pin_val) | (1 << (pin)))

// 将指定的bit位置0
#define BIT_CLEAR(pin_val, pin) ((pin_val) & ~(1 << (pin)))

// 读取指定的bit位
#define BIT_READ(pin_val, pin) (((pin_val) >> (pin)) & 0x01)

// 根据val的值将指定的bit位置1或0
#define BIT_WRITE(pin, pin_val, val) ((val) ? BIT_SET((pin_val), (pin)) : BIT_CLEAR((pin_val), (pin)))


/*
初始化PCA9557,0x18
引脚                               7    6     5    4     3      2      1      0
0x03寄存器 0: 输出模式 1: 输入模式   NC   NC    NC  SHUT  MUTE  TP_RST  LED_BK  NC
配置寄存器0x03写0xE1--->            1    1     1    0     0      0      0      1
设置输出寄存器初始值，写0x01输出寄存器0x00
*/
void pca9557_init() {
  LOG_DEBUG();
//  Serial.println("pca9557_init");
  Wire.beginTransmission(ADDRESS);
  Wire.write(CONFIG_REG);
  Wire.write(0xE1);
//  Wire.write(OUTPUT_REG);
//  Wire.write(0x00);
  Wire.endTransmission();
}
int _pca9557_readInputReg() {
  Wire.beginTransmission(ADDRESS);
  Wire.write(INPUT_REG);
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS, (uint8_t)1);
  if(Wire.available()) {
    return Wire.read();
  }
  return -1;
}
int _pca9557_readOutputReg() {
  Wire.beginTransmission(ADDRESS);
  Wire.write(OUTPUT_REG);
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS, (uint8_t)1);
  if(Wire.available()) {
    return Wire.read();
  }
  return -1;
}
int pca9557_onePinWrite(uint8_t pin, uint8_t val) {
  LOG_DEBUG();
  /* 首先读取输出寄存器的值，保证后续的置位操作不影响其他的输出引脚 */
  int value = _pca9557_readOutputReg();
  if(-1 == value) Serial.println("read pca9557 error");
  /* 将对应的引脚置位操作 */
  value = BIT_WRITE(pin, value, val);
  /* 回写输出寄存器 */
  Wire.beginTransmission(ADDRESS);
  Wire.write(OUTPUT_REG);
  Wire.write(value);
  Wire.endTransmission();
}
