#include "pins_config.h"
#include "LovyanGFX_Driver.h"
#include <lvgl.h>
#include "demos/lv_demos.h"
#include <Wire.h>
#include <SPI.h>
#include "FS.h"
#include "SD.h"
#include "I2C_BM8563.h"
#include <WiFi.h>
#include <time.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "BLEDevice.h"  //BLE驱动库
#include "BLEServer.h"  //BLE蓝牙服务器库
#include "BLEUtils.h"   //BLE实用程序库
#include "BLE2902.h"    //特征添加描述符库
#include "Arduino.h"
#include "WiFiMulti.h"
#include "Audio.h"
#include "ESP_I2S.h"

// 麦克风引脚定义（录音使用左声道）
#define I2S_WS_MIC 2    // WS
#define I2S_SCK_MIC 19  // SCK
#define I2S_SD_MIC 20   // SD (数据输入)

// 喇叭引脚定义（播放使用右声道）
#define I2S_DOUT_SPK 4  // 喇叭数据输出
#define I2S_LRC_SPK 6   // 声道时钟（WS/LRC）
#define I2S_SPK_BCLK 5  // 位时钟 (BCLK)

// 采样配置参数
const uint32_t SAMPLE_RATE = 16000;                          // 采样率 (Hz)
const i2s_data_bit_width_t BITS = I2S_DATA_BIT_WIDTH_16BIT;  // 采样位宽 (16bit)
// 为了进行左右声道录放，均设置为立体声模式
const int CHANNEL_STEREO = 2;

// 录音时长（秒）
const int RECORD_SECONDS = 5;
// 录音时的缓冲区大小计算：采样率 × 录音秒数 × (位宽/8) × 立体声(2)
// 注意：由于后续通过转换只保留左声道，因此实际得到的单声道数据字节数是该值的一半
const size_t REC_BUFFER_SIZE = SAMPLE_RATE * RECORD_SECONDS * (BITS / 8) * CHANNEL_STEREO;

// 声音处理参数
const float GAIN = 50.0f;  // 放大倍数（例如放大2倍）
const float ALPHA = 0.1f;  // IIR滤波器平滑系数（0~1之间），值越小滤波越平滑

// 用于存放转换后仅含左声道的录音数据（单声道，16bit）
uint8_t *audioBuffer = nullptr;
uint8_t *playBuffer = nullptr;


BLECharacteristic *pCharacteristic;
BLEServer *pServer;
BLEService *pService;
bool deviceConnected = false;
char BLEbuf[32] = { 0 };

#define OLED_RESET -1
#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* RTC BM8563 I2C port */
#define BM8563_I2C_SDA 15
#define BM8563_I2C_SCL 16
I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire);

//射频2401
#include <nRF24L01.h>
#include <RF24.h>
#define CE_PIN 20  //txrx 复用
#define CSN_PIN 19
// instantiate an object for the nRF24L01 transceiver
RF24 rf_radio(CE_PIN, CSN_PIN);

/* 音频模块 */
WiFiMulti wifiMulti;
Audio audio;
bool audioIsInited = false;

/* 扩展io */
//#include <TCA9534.h>
//TCA9534 ioex;

#define HSPI_MISO 4
#define HSPI_MOSI 6
#define HSPI_SCLK 5
#define HSPI_SS 19

#define SD_MOSI 6
#define SD_MISO 4
#define SD_SCK 5
#define SD_CS 0  //The chip selector pin is not connected to IO

#define I2S_DOUT 4
#define I2S_BCLK 5
#define I2S_LRC 6

#define LOG_DEBUG() \
  do { \
    Serial.print("FUNCTION["); \
    Serial.print(__FUNCTION__); \
    Serial.print("]\tLINE["); \
    Serial.print(__LINE__); \
    Serial.println("]"); \
  } while (0)

SPIClass *hspi = nullptr;

const char *ntpServer = "time.cloudflare.com";

LGFX gfx;

void show_test(int lcd_w, int lcd_h, int x, int y, const char *text) {
  gfx.fillScreen(TFT_BLACK);
  gfx.setTextSize(3);
  gfx.setTextColor(TFT_RED);
  gfx.setCursor(x, y);
  gfx.print(text);  // 显示文本
}

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"  // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
    Serial.println("------> BLE connect .");
    show_test(800, 480, 300, 230, "BLE connect");
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
    Serial.println("------> BLE disconnect .");
    show_test(800, 480, 300, 230, "BLE disconnect");
    pServer->startAdvertising();  // restart advertising
    Serial.println("start advertising");
  }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      Serial.print("------>Received Value: ");

      for (int i = 0; i < rxValue.length(); i++) {
        Serial.print(rxValue[i]);
      }
      Serial.println();

      if (rxValue.indexOf("A") != -1) {
        Serial.print("Rx A!");
      } else if (rxValue.indexOf("B") != -1) {
        Serial.print("Rx B!");
      }
      Serial.println();
    }
  }
};

bool test_flag;  //测试程序判断

#include <Ticker.h>  //Call the ticker. H Library
Ticker ticker1;
Ticker ticker_getRTC;

//UI
#include "ui.h"
static int first_flag = 0;
extern int zero_clean;
extern int goto_widget_flag;
extern int bar_flag;
extern lv_obj_t *ui_MENU;
extern lv_obj_t *ui_TOUCH;
extern lv_obj_t *ui_JIAOZHUN;
extern lv_obj_t *ui_Label2;
static lv_obj_t *ui_Label;   //TOUCH界面label
static lv_obj_t *ui_Label3;  //TOUCH界面label3
static lv_obj_t *ui_Labe2;   //Menu界面进度条label
static lv_obj_t *bar;        //Menu界面进度条

/* Change to your screen resolution */
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;
static lv_color_t *buf1;

uint16_t touch_x, touch_y;

//显示刷新
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  if (gfx.getStartCount() > 0) {
    gfx.endWrite();
  }
  gfx.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::rgb565_t *)&color_p->full);

  lv_disp_flush_ready(disp);  //告诉lvgl刷新完成
}

//读取触摸
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  data->state = LV_INDEV_STATE_REL;
  bool touched = gfx.getTouch(&touch_x, &touch_y);
  if (touched) {
    data->state = LV_INDEV_STATE_PR;

    //设置坐标
    data->point.x = touch_x;
    data->point.y = touch_y;
  }
}

static int val = 100;
void callback1()  //Callback function
{
  if (bar_flag == 6) {
    if (val > 1) {
      val--;
      lv_bar_set_value(bar, val, LV_ANIM_OFF);
      lv_label_set_text_fmt(ui_Labe2, "%d %%", val);
    } else {
      lv_obj_clear_flag(ui_touch, LV_OBJ_FLAG_CLICKABLE);
      lv_label_set_text(ui_Labe2, "Loading");
      delay(150);
      val = 100;
      bar_flag = 0;          //停止进度条标志
      goto_widget_flag = 1;  //进入widget标志
    }
  }
}

void calibrateTouch(uint16_t *parameters, uint32_t color_fg, uint32_t color_bg, uint8_t size) {
  int16_t values[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  uint16_t x_tmp, y_tmp;
  uint16_t _width = 800;
  uint16_t _height = 480;

  for (uint8_t i = 0; i < 4; i++) {
    gfx.fillRect(0, 0, size + 1, size + 1, color_bg);
    gfx.fillRect(0, _height - size - 1, size + 1, size + 1, color_bg);
    gfx.fillRect(_width - size - 1, 0, size + 1, size + 1, color_bg);
    gfx.fillRect(_width - size - 1, _height - size - 1, size + 1, size + 1, color_bg);

    switch (i) {
      case 0:  // up left
        gfx.fillRect(0, 0, size + 1, size + 1, color_fg);
        break;
      case 1:  // bot left
        gfx.fillRect(0, _height - size - 1, size + 1, size + 1, color_fg);
        break;
      case 2:  // up right
        gfx.fillRect(_width - size - 1, 0, size + 1, size + 1, color_fg);
        break;
      case 3:  // bot right
        gfx.fillRect(_width - size - 1, _height - size - 1, size + 1, size + 1, color_fg);
        break;
    }

    // user has to get the chance to release
    if (i > 0) delay(1000);

    for (uint8_t j = 0; j < 8; j++) {
      while (1) {
        bool touched = gfx.getTouch(&touch_x, &touch_y);
        if (touched) {
          Serial.print("Data x :");
          Serial.println(touch_x);
          Serial.print("Data y :");
          Serial.println(touch_y);
          break;
        }
      }
    }
  }
}

void touch_calibrate()  //屏幕校准
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;
  Serial.println("屏幕校准");

  Serial.println("touch_calibrate ...");

  lv_timer_handler();
  calibrateTouch(calData, TFT_RED, TFT_BLACK, 17);
  Serial.println("calibrateTouch(calData, TFT_RED, TFT_BLACK, 15)");
  Serial.println();
  Serial.println();
  Serial.println("//在setup()use code:");
  Serial.print("uint16_t calData[5] = ");
  Serial.print("{ ");

  for (uint8_t i = 0; i < 5; i++) {
    Serial.print(calData[i]);
    if (i < 4) Serial.print(", ");
  }

  Serial.println(" };");
}

//触摸Label控件
void label_xy() {
  ui_Label = lv_label_create(ui_TOUCH);
  lv_obj_enable_style_refresh(true);
  lv_obj_set_width(ui_Label, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_height(ui_Label, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_x(ui_Label, -55);
  lv_obj_set_y(ui_Label, -40);
  lv_obj_set_align(ui_Label, LV_ALIGN_CENTER);
  lv_obj_set_style_text_color(ui_Label, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(ui_Label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui_Label, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

  ui_Label3 = lv_label_create(ui_TOUCH);
  lv_obj_enable_style_refresh(true);
  lv_obj_set_width(ui_Label3, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_height(ui_Label3, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_x(ui_Label3, 85);
  lv_obj_set_y(ui_Label3, -40);
  lv_obj_set_align(ui_Label3, LV_ALIGN_CENTER);
  lv_obj_set_style_text_color(ui_Label3, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(ui_Label3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui_Label3, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
}


//进度条控件
void lv_example_bar(void) {
  //////////////////////////////
  bar = lv_bar_create(ui_MENU);
  lv_bar_set_value(bar, 0, LV_ANIM_OFF);
  lv_obj_set_width(bar, 480);
  lv_obj_set_height(bar, 25);
  lv_obj_set_x(bar, 0);
  lv_obj_set_y(bar, 205);
  lv_obj_set_align(bar, LV_ALIGN_CENTER);
  lv_obj_set_style_bg_img_src(bar, &ui_img_bar_800_01_png, LV_PART_MAIN | LV_STATE_DEFAULT);

  lv_obj_set_style_bg_img_src(bar, &ui_img_bar_800_02_png, LV_PART_INDICATOR | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_color(bar, lv_color_hex(0x2D8812), LV_PART_INDICATOR | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_opa(bar, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
  //////////////////////
  ui_Labe2 = lv_label_create(bar);  //创建标签
  lv_obj_set_style_text_color(ui_Labe2, lv_color_hex(0x09BEFB), LV_STATE_DEFAULT);
  lv_label_set_text(ui_Labe2, "0%");
  lv_obj_center(ui_Labe2);
}

void get_ntp_BM8563_Test() {
  // Set ntp time to local
  configTime(8 * 3600, 0, ntpServer);  //China time
  printLocalTime();
}

bool i2cScanForAddress(uint8_t address) {
  Wire.beginTransmission(address);
  return (Wire.endTransmission() == 0);
}
void printLocalTime() {
  // Init I2C
  Wire.begin(BM8563_I2C_SDA, BM8563_I2C_SCL);

  // // Init RTC
  rtc.begin();

  struct tm timeInfo;
  if (!getLocalTime(&timeInfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  // 格式化并打印时间
  Serial.print("NTP Time:  ");
  Serial.println(&timeInfo, "%A, %B %d %Y %H:%M:%S");

  // Set RTC time
  I2C_BM8563_TimeTypeDef timeStruct;
  timeStruct.hours = timeInfo.tm_hour;
  timeStruct.minutes = timeInfo.tm_min;
  timeStruct.seconds = timeInfo.tm_sec;
  rtc.setTime(&timeStruct);

  // Set RTC Date
  I2C_BM8563_DateTypeDef dateStruct;
  dateStruct.weekDay = timeInfo.tm_wday;
  dateStruct.month = timeInfo.tm_mon + 1;
  dateStruct.date = timeInfo.tm_mday;
  dateStruct.year = timeInfo.tm_year + 1900;
  rtc.setDate(&dateStruct);

  // Serial.printf("[INFO-%d] weekDay: %d-mouth: %d-date: %d-year: %d\thours: %d\tminutes:%d\tseconds:%d\n",
  //                     dateStruct.weekDay,
  //                     dateStruct.month,
  //                     dateStruct.date,
  //                     dateStruct.year,
  //                     timeStruct.hours,
  //                     timeStruct.minutes,
  //                     timeStruct.seconds);

  get_bm8563_time();
}

void get_bm8563_time() {
  Wire.begin(BM8563_I2C_SDA, BM8563_I2C_SCL);

  // Init RTC
  rtc.begin();

  I2C_BM8563_DateTypeDef dateStruct;
  I2C_BM8563_TimeTypeDef timeStruct;

  // Get RTC
  rtc.getDate(&dateStruct);
  rtc.getTime(&timeStruct);

  // Print RTC
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\n",
                dateStruct.year,
                dateStruct.month,
                dateStruct.date,
                timeStruct.hours,
                timeStruct.minutes,
                timeStruct.seconds);
}

void buzzer_test() {
  // analogWrite(8, 249);
  // 开始向地址 0x30 发送命令 0x15
  Wire.beginTransmission(0x30);            // 7 位地址 0x30
  Wire.write(0x15);                        // 发送命令 0
  uint8_t error = Wire.endTransmission();  // 结束传输并返回状态

  if (error == 0) {
    Serial.println("命令 0x15 发送成功");
  } else {
    Serial.print("命令发送错误，错误代码：");
    Serial.println(error);
  }
}

void buzzer_stop() {
  // analogWrite(8, 0);
  // 开始向地址 0x30 发送命令 0x15
  Wire.beginTransmission(0x30);            // 7 位地址 0x30
  Wire.write(0x16);                        // 发送命令 0
  uint8_t error = Wire.endTransmission();  // 结束传输并返回状态

  if (error == 0) {
    Serial.println("命令 0x16 发送成功");
  } else {
    Serial.print("命令发送错误，错误代码：");
    Serial.println(error);
  }
}

void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(2);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(25, 25);            // Start at top-left corner
  display.println(F("ELECROW"));
  display.display();
  //  delay(2000);
}

void oled_test() {
  //初始化显示屏
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  testdrawstyles();
}

void oled_stop() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.display();
}

void wifi_test() {
  char command[64] = { 0 };  //串口收到的命令
  char count = 0;            //命令的长度
  bool flag = false;
  bool timeout_flag = false;
  while (1) {
    while (Serial.available()) {
      String ssid = Serial.readStringUntil(',');       // 读取 SSID
      String password = Serial.readStringUntil('\n');  // 读取密码
      // 在这里处理 SSID 和密码
      Serial.print("SSID: ");
      Serial.println(ssid);
      Serial.print("Password: ");
      Serial.println(password);
      WiFi.begin(ssid.c_str(), password.c_str());
      int timeout = 0;
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        timeout++;
        if (timeout == 40) {
          Serial.println("Connection timeout exit");
          Serial.println("EXIT WIFI test");
          char command[64] = { 0 };  //串口收到的命令
          char count = 0;            //命令的长度
          return;
        }
      }
      if (timeout_flag == false) {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());

        show_test(800, 480, 300, 230, "WiFi connected");
        Serial.println("Update the RTC time");
        get_ntp_BM8563_Test();  //get NTP time

        char command[64] = { 0 };  //串口收到的命令
        char count = 0;            //命令的长度
        flag = true;
      }
    }
    if (flag == true) {
      Serial.println("EXIT WIFI test");
      WiFi.disconnect();
      char command[64] = { 0 };  //串口收到的命令
      char count = 0;            //命令的长度
      break;
    }
    delay(10);
  }
}

void ble_test() {
  char command[64] = { 0 };  //串口收到的命令
  char count = 0;            //命令的长度

  // 开始蓝牙服务
  pService->start();
  // 开始广播
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");

  while (1) {
    while (Serial.available()) {
      val = Serial.read();
      command[count] = val;
      count++;
    }
    if (command[0] == 'b') {
      Serial.println("EXIT ble test");
      gfx.fillScreen(BLACK);
      pServer->getAdvertising()->stop();  //停止广播
      pService->stop();
      break;
    }

    if (deviceConnected) {  //设备连接后，每秒钟发送txValue。
      memset(BLEbuf, 0, 32);
      memcpy(BLEbuf, (char *)"Hello BLE APP!", 32);
      pCharacteristic->setValue(BLEbuf);

      pCharacteristic->notify();  // Send the value to the app!
      Serial.print("*** Sent Value: ");
      Serial.print(BLEbuf);
      Serial.println(" ***");
    }
    delay(1000);
  }
}

int lcd_test() {
  char command[64] = { 0 };  //串口收到的命令
  char count = 0;            //命令的长度
  int i = 0;
  while (1) {
    while (Serial.available()) {
      val = Serial.read();
      command[count] = val;
      count++;
    }
    if (command[0] == 'n') {
      Serial.println("auto test break");
      gfx.fillScreen(TFT_BLACK);
      return 1;
    }

    switch (i) {
      case 0:
        Serial.println("RED");
        gfx.fillScreen(TFT_RED);
        delay(2000);
        break;
      case 1:
        Serial.println("GREEN");
        gfx.fillScreen(TFT_GREEN);
        delay(2000);
        break;
      case 2:
        Serial.println("BLUE");
        gfx.fillScreen(TFT_BLUE);
        delay(2000);
        break;
      case 3:
        Serial.println("WHITE");
        gfx.fillScreen(TFT_WHITE);
        delay(2000);
        break;
      case 4:
        Serial.println("GRAY");
        gfx.fillScreen(TFT_DARKGREY);  // 0x8410
        delay(2000);
        Serial.println("Exit display finish");
        gfx.fillScreen(TFT_BLACK);
        return 0;
    }

    i++;
  }
}

/* 下位机触摸测试 */
void touch_test() {
  char command[64] = { 0 };  //串口收到的命令
  char count = 0;            //命令的长度
  uint16_t last_x = 0;
  uint16_t last_y = 0;
  while (1) {
    while (Serial.available()) {
      val = Serial.read();
      command[count] = val;
      count++;
    }
    if (command[0] == 'm') {
      Serial.println("EXIT Touch test");
      gfx.fillScreen(TFT_BLACK);
      break;
    }
    bool touched = gfx.getTouch(&touch_x, &touch_y);
    // Serial.printf("x[%d]\ty[%d]\n", touch_x, touch_y);
    // Serial.printf("\n[INFO]--->touched is [%s]\n", touched ? "True" : "False");
    if (touched && (last_x != touch_x || last_y != touch_y)) {
      last_x = touch_x;
      last_y = touch_y;
      Serial.print("x: ");
      Serial.print(touch_x);
      Serial.print("    ");
      Serial.print("y: ");
      Serial.println(touch_y);
    }
  }
}

//SD卡初始化
void SD_init() {
  hspi = new SPIClass(HSPI);  // by default VSPI is used
  hspi->begin(SD_SCK, SD_MISO, SD_MOSI);
  if (!SD.begin(SD_CS, *hspi, 80000000)) {
    Serial.println(F("ERROR: File system mount failed!"));
    hspi->end();
  } else {
    Serial.printf("SD Size: %lluMB \n", SD.cardSize() / (1024 * 1024));
    hspi->end();
  }
}

//初始化2401测试 SPI txrx也能测，因为复用
void spi_test() {
  hspi = new SPIClass(HSPI);  // by default VSPI is used
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);
  if (!rf_radio.begin(hspi)) {
    Serial.println("rf_radio hardware is not responding");
    hspi->end();
  } else {
    Serial.println("rf_radio hardware is OK");
    hspi->end();
  }
}

#include "lora_pingpong.hpp"
/* lora相关变量 */
// ESP32 - SX126x pin configuration
int PIN_LORA_RESET = 19;  // LORA RESET
int PIN_LORA_DIO_1 = 20;  // LORA DIO_1
int PIN_LORA_BUSY = 2;    // LORA SPI BUSY
int PIN_LORA_NSS = 8;     // LORA SPI CS
int PIN_LORA_SCLK = 5;    // LORA SPI CLK
int PIN_LORA_MISO = 4;    // LORA SPI MISO
int PIN_LORA_MOSI = 6;    // LORA SPI MOSI
int RADIO_TXEN = -1;      // LORA ANTENNA TX ENABLE
int RADIO_RXEN = -1;      // LORA ANTENNA RX ENABLE
// static RadioEvents_t RadioEvents;
// static uint16_t BufferSize = BUFFER_SIZE;
// static uint8_t RcvBuffer[BUFFER_SIZE];
// static uint8_t TxdBuffer[BUFFER_SIZE];
// static bool isMaster = true;
// const uint8_t PingMsg[] = "PING";
// const uint8_t PongMsg[] = "PONG";
// time_t timeToSend;
// time_t cadTime;
// uint8_t pingCnt = 0;
// uint8_t pongCnt = 0;

hw_config hwConfig;
/* 无线模块lora测试 */
void lora_test() {
  /* lora配置 */
  hwConfig.CHIP_TYPE = SX1262_CHIP;          // Example uses an eByte E22 module with an SX1262
  hwConfig.PIN_LORA_RESET = PIN_LORA_RESET;  // LORA RESET
  hwConfig.PIN_LORA_NSS = PIN_LORA_NSS;      // LORA SPI CS
  hwConfig.PIN_LORA_SCLK = PIN_LORA_SCLK;    // LORA SPI CLK
  hwConfig.PIN_LORA_MISO = PIN_LORA_MISO;    // LORA SPI MISO
  hwConfig.PIN_LORA_DIO_1 = PIN_LORA_DIO_1;  // LORA DIO_1
  hwConfig.PIN_LORA_BUSY = PIN_LORA_BUSY;    // LORA SPI BUSY
  hwConfig.PIN_LORA_MOSI = PIN_LORA_MOSI;    // LORA SPI MOSI
  hwConfig.RADIO_TXEN = RADIO_TXEN;          // LORA ANTENNA TX ENABLE
  hwConfig.RADIO_RXEN = RADIO_RXEN;          // LORA ANTENNA RX ENABLE
  hwConfig.USE_DIO2_ANT_SWITCH = true;       // Example uses an CircuitRocks Alora RFM1262 which uses DIO2 pins as antenna control
  hwConfig.USE_DIO3_TCXO = true;             // Example uses an CircuitRocks Alora RFM1262 which uses DIO3 to control oscillator voltage
  hwConfig.USE_DIO3_ANT_SWITCH = false;      // Only Insight ISP4520 module uses DIO3 as antenna control

  Serial.println("=====================================");
  Serial.println("lora1262 ID test");
  Serial.println("=====================================");

  Serial.println("MCU Espressif ESP32");
  uint8_t deviceId[8] = { 0 };

  BoardGetUniqueId(deviceId);
  Serial.printf("BoardId: %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n",
                deviceId[7],
                deviceId[6],
                deviceId[5],
                deviceId[4],
                deviceId[3],
                deviceId[2],
                deviceId[1],
                deviceId[0]);
  // Initialize the LoRa chip
  Serial.println("Starting lora_hardware_init");
  uint32_t ret = lora_hardware_init(hwConfig);
  if (ret == 0) {
    Serial.println("[INFO] Successful initialisation of the lora also means that it is able to communicate successfully");
  } else if (ret == 1) {
    Serial.println("[ERR] Failure to initialise lora also means communication failure");
  }

  // // Initialize the Radio callbacks
  // RadioEvents.TxDone = OnTxDone;
  // RadioEvents.RxDone = OnRxDone;
  // RadioEvents.TxTimeout = OnTxTimeout;
  // RadioEvents.RxTimeout = OnRxTimeout;
  // RadioEvents.RxError = OnRxError;
  // RadioEvents.CadDone = OnCadDone;

  // // Initialize the Radio
  // Radio.Init(&RadioEvents);

  // // Set Radio channel
  // Radio.SetChannel(RF_FREQUENCY);

  // // Set Radio TX configuration
  // Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
  // 				  LORA_SPREADING_FACTOR, LORA_CODINGRATE,
  // 				  LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
  // 				  true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);

  // // Set Radio RX configuration
  // Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
  // 				  LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
  // 				  LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
  // 				  0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

  // // Start LoRa
  // Serial.println("Starting Radio.Rx");
  // Radio.Rx(RX_TIMEOUT_VALUE);
}

//  喇叭测试
void speaker_test() {

  WiFi.mode(WIFI_STA);
  char command[64] = { 0 };  //串口收到的命令
  char count = 0;            //命令的长度
  bool flag = false;
  bool timeout_flag = false;
  bool readyToPlay = false;
  delay(1000);
  while (1) {
    while (Serial.available()) {
      String ssid = Serial.readStringUntil(',');      //  读取SSID
      String password = Serial.readStringUntil(',');  //  读取密码
      String urlStr = Serial.readStringUntil('\n');   //  读取音源url地址
      // 在这里处理 SSID 和密码
      Serial.print("[SSID]: ");
      Serial.println(ssid);
      Serial.print("[Password]: ");
      Serial.println(password);
      Serial.print("[URL]: ");
      Serial.println(urlStr);
      wifiMulti.addAP(ssid.c_str(), password.c_str());
      wifiMulti.run();
      int timeout = 0;
      while (WiFi.status() != WL_CONNECTED) {
        // while (!timeout_flag) {
        delay(100);
        Serial.print(".");
        timeout++;
        WiFi.disconnect(true);
        wifiMulti.run();
        if (timeout == 20) {
          Serial.println("Connection timeout exit");
          Serial.println("EXIT WIFI test");
          gfx.fillScreen(TFT_WHITE);
          char command[64] = { 0 };  //串口收到的命令
          char count = 0;            //命令的长度
          timeout_flag = true;
          return;
        }
      }
      if (timeout_flag == false) {
        Serial.println("");
        Serial.println("[INFO] WiFi connected");
        /* 音频只初始化一次 */
        if (audioIsInited == false) {
          audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
          audio.setVolume(14);  // 0...21
          Serial.println("[INFO] Audio init success!");
          audioIsInited = true;
        }
        // audio.connecttohost("http://music.163.com/song/media/outer/url?id=2086327879.mp3"); //鲜花.mp3b
        // audio.connecttohost("http://music.163.com/song/media/outer/url?id=5103312.mp3"); //Empire state of mine.mp3

        audio.connecttohost(urlStr.c_str());
        show_test(800, 480, 300, 230, "Ready to play music");
        Serial.println("Ready to play music");
        /* 网络连接完毕，可以进入播放循环中 */
        readyToPlay = true;
        char command[64] = { 0 };  //串口收到的命令
        char count = 0;            //命令的长度
      }
    }

    if (readyToPlay) {
      /* 将控制静音的引脚拉低，打开声音 */
      //ioex.output(4, TCA9534::Level::L);
      // 开始向地址 0x30 发送命令关闭静音
      Wire.beginTransmission(0x30);            // 7 位地址 0x30
      Wire.write(0x17);                        // 发送命令 0x10
      uint8_t error = Wire.endTransmission();  // 结束传输并返回状态

      if (error == 0) {
        Serial.println("命令 0x17 发送成功");
      } else {
        Serial.print("命令发送错误，错误代码：");
        Serial.println(error);
      }
      while (1) {
        audio.loop();
        if (Serial.available()) {
          audio.stopSong();
          String r = Serial.readString();
          r.trim();
          if (r.length() > 5) {
            Serial.println("[INFO] Switch to another song");
            audio.connecttohost(r.c_str());
          } else {
            flag = true;
            break;
          }
        }
      }
    }
    if (flag == true) {
      Serial.println("EXIT Speaker test");
      //ioex.output(4, TCA9534::Level::H);  //  静音
      // 开始向地址 0x30 发送命令开启静音
      Wire.beginTransmission(0x30);            // 7 位地址 0x30
      Wire.write(0x17);                        // 发送命令 0x10
      uint8_t error = Wire.endTransmission();  // 结束传输并返回状态

      if (error == 0) {
        Serial.println("命令 0x17 发送成功");
      } else {
        Serial.print("命令发送错误，错误代码：");
        Serial.println(error);
      }
      gfx.fillScreen(TFT_BLACK);
      break;
    }
    delay(10);
  }
}
void processAudio(int16_t *samples, size_t sampleCount, float gain, float alpha) {
  // 处理第一个采样
  float prev = samples[0] * gain;
  if (prev > 32767.0f) prev = 32767.0f;
  if (prev < -32768.0f) prev = -32768.0f;
  samples[0] = (int16_t)prev;

  // 对后续采样应用一阶IIR低通滤波器
  for (size_t i = 1; i < sampleCount; i++) {
    float amplified = samples[i] * gain;
    float filtered = alpha * amplified + (1.0f - alpha) * prev;
    if (filtered > 32767.0f) filtered = 32767.0f;
    if (filtered < -32768.0f) filtered = -32768.0f;
    samples[i] = (int16_t)filtered;
    prev = filtered;
  }
}
void Mic_init() {
  // 创建一个 I2SClass 实例
  I2SClass i2s;

  // 创建用于存储音频数据的变量
  uint8_t *wav_buffer;
  size_t wav_size;
// 初始化I2C并发送解除静音指令
  Serial.println("初始化I2C并解除静音...");

  
  Wire.beginTransmission(0x30);
  Wire.write(0x0);
  Wire.write(0x17);
  uint8_t error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.println("解除静音命令发送成功");
  } else {
    Serial.print("命令发送错误，错误代码：");
    Serial.println(error);
    
    while(error != 0) {
      Wire.beginTransmission(0x30);
      Wire.write(0x17);
      error = Wire.endTransmission();
      
      if (error == 0) {
        Serial.println("解除静音命令发送成功");
      } else {
        Serial.print("命令发送错误，错误代码：");
        Serial.println(error);
      }
      delay(100);
    }
  }

  Serial.println("正在初始化 I2S 总线...");

  // 设置用于音频输入的引脚
  i2s.setPinsPdmRx(19, 20);

  // 以 16 kHz 频率及 16 位深度单声道启动 I2S
  if (!i2s.begin(I2S_MODE_PDM_RX, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("初始化 I2S 失败！");
    while (1); // 不执行任何操作
  }

  Serial.println("I2S 总线已初始化。");
  // Serial.println("正在初始化 SD 卡...");

  // 设置用于访问 SD 卡的引脚
  // SPI.begin(5, 4, 6, -1);
  // if(!SD.begin(10)){
  //   Serial.println("挂载 SD 卡失败！");
  //   while (1) ;
  // }
  // Serial.println("SD 卡已初始化。");
  Serial.println("正在录制 5 秒的音频数据...");



  // 录制 5 秒的音频数据
  wav_buffer = i2s.recordWAV(5, &wav_size);

 
  
  // 重新配置I2S为输出模式
  i2s.end(); // 先结束当前I2S配置
  
  // 使用定义的喇叭引脚设置I2S输出
  i2s.setPins(I2S_SPK_BCLK, I2S_LRC_SPK, I2S_DOUT_SPK); // BCLK, LRCLK, DOUT
  
  // 以相同的参数启动I2S，但改为输出模式，使用立体声（右声道播放）
  if (!i2s.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("初始化I2S输出模式失败！");
    while (1);
  }
  
  Serial.println("正在播放录制的音频...");
  
  // 跳过WAV文件头（通常为44字节）
uint8_t *audio_data = wav_buffer + 44;
size_t audio_size = wav_size - 44;

// 创建立体声缓冲区（左声道静音，右声道播放）
uint8_t *stereo_buffer = (uint8_t*)heap_caps_malloc(audio_size * 2, MALLOC_CAP_SPIRAM);
if (stereo_buffer == NULL) {
  Serial.println("内存分配失败！");
  return;
}

// 定义放大系数（例如：2.0表示放大两倍）
float amplification = 10.0;

// 将单声道数据转换为立体声数据（左声道为0，右声道为放大后的原始数据）
for (size_t i = 0, j = 0; i < audio_size; i += 2, j += 4) {
  // 左声道数据（静音）
  stereo_buffer[j] = 0;
  stereo_buffer[j + 1] = 0;
  
  // 获取原始16位音频样本
  int16_t sample = (int16_t)((audio_data[i+1] << 8) | audio_data[i]);
  
  // 放大样本值，并防止溢出
  float amplified_sample = sample * amplification;
  if (amplified_sample > 32767) amplified_sample = 32767;
  if (amplified_sample < -32768) amplified_sample = -32768;
  
  // 转换回字节并存储到右声道
  int16_t new_sample = (int16_t)amplified_sample;
  stereo_buffer[j + 2] = new_sample & 0xFF;
  stereo_buffer[j + 3] = (new_sample >> 8) & 0xFF;
}

// 播放音频数据
size_t bytes_written = 0;
// 修改 write 方法的调用方式
i2s.write(stereo_buffer, audio_size * 2);
  // 释放内存
  heap_caps_free(stereo_buffer);

  free(wav_buffer);

  // 结束I2S
  i2s.end();
  
  Serial.println("音频播放完成。");
  Serial.println("程序执行完成。");
  Serial.println("检测到串口数据输入，已退出循环");
}
// void Mic_init() {
//   I2SClass i2sMic;      // 用于录音（RX）
//   I2SClass i2sSpeaker;  // 用于播放（TX）
//   Serial.println("初始化I2S麦克风（录音左声道）...");
//   // 配置麦克风：设置为立体声模式（用于后续转换）
//   i2sMic.setPins(I2S_SCK_MIC, I2S_WS_MIC, -1, I2S_SD_MIC, -1);
//   if (!i2sMic.begin(I2S_MODE_STD, SAMPLE_RATE, BITS, (i2s_slot_mode_t)CHANNEL_STEREO)) {
//     Serial.println("麦克风I2S初始化失败！");
//     return;
//   }
//   // 配置 RX 转换（立体声转单声道）
//   if (!i2sMic.configureRX(SAMPLE_RATE, BITS, (i2s_slot_mode_t)CHANNEL_STEREO, I2S_RX_TRANSFORM_16_STEREO_TO_MONO)) {
//     Serial.println("麦克风配置RX转换失败！");
//     return;
//   }

//   Serial.println("初始化I2S喇叭（播放右声道）...");
//   i2sSpeaker.setPins(I2S_SPK_BCLK, I2S_LRC_SPK, I2S_DOUT_SPK, -1, -1);
//   if (!i2sSpeaker.begin(I2S_MODE_STD, SAMPLE_RATE, BITS, (i2s_slot_mode_t)CHANNEL_STEREO)) {
//     Serial.println("喇叭I2S初始化失败！");
//     return;
//   }

//   // 主循环：5秒录音+播放循环
//   while (Serial.available() == 0) {  // 当串口无数据时循环
//     // 计算5秒录音数据量（单声道）
//     const size_t recordDurationMs = 5000;  // 5秒
//     const size_t bytesToRead = SAMPLE_RATE * (recordDurationMs / 1000.0) * 2;  // 单声道16位

//     // 分配录音缓冲区（外部SPIRAM）
//     uint8_t* audioBuffer = (uint8_t*)heap_caps_malloc(bytesToRead, MALLOC_CAP_SPIRAM);
//     if (!audioBuffer) {
//       Serial.println("录音内存分配失败！");
//       break;
//     }

//     // 录音操作
//     Serial.println("\n开始5秒录音...");
//     size_t bytesRead = i2sMic.readBytes((char*)audioBuffer, bytesToRead);
//     if (bytesRead != bytesToRead) {
//       Serial.printf("录音数据不足！需要%d字节，实际%d字节\n", bytesToRead, bytesRead);
//       heap_caps_free(audioBuffer);
//       continue;
//     }

//     // 音频处理
//     size_t monoSampleCount = bytesRead / 2;
//     processAudio((int16_t*)audioBuffer, monoSampleCount, GAIN, ALPHA);

//     // 转换单声道到立体声（右声道输出）
//     const size_t playBufferSize = monoSampleCount * 4;  // 立体声16位：样本数 × 4字节
//     uint8_t* playBuffer = (uint8_t*)heap_caps_malloc(playBufferSize, MALLOC_CAP_SPIRAM);
//     if (!playBuffer) {
//       Serial.println("播放内存分配失败！");
//       heap_caps_free(audioBuffer);
//       break;
//     }

//     // 填充立体声数据
//     int16_t* src = (int16_t*)audioBuffer;
//     int16_t* dst = (int16_t*)playBuffer;
//     for (size_t i = 0; i < monoSampleCount; i++) {
//       dst[2 * i] = 0;       // 左声道静音
//       dst[2 * i + 1] = src[i];  // 右声道填充数据
//     }

//     // 播放操作
//     Serial.println("开始播放...");
//     size_t bytesWritten = i2sSpeaker.write(playBuffer, playBufferSize);
//     Serial.printf("播放完成，写入字节数：%d\n", bytesWritten);

//     // 释放本次循环内存
//     heap_caps_free(audioBuffer);
//     heap_caps_free(playBuffer);
//   }

//   // 清理资源
//   i2sMic.end();
//   i2sSpeaker.end();
//   Serial.println("检测到串口数据输入，已退出循环");
// }
// void Mic_init() {

//   Serial.println("初始化I2S麦克风（录音左声道）...");
//   // 配置麦克风：设置为立体声模式（用于后续转换），
//   // 参数顺序：bclk, ws, dout（此处不用，传-1），din, mclk（暂不用传-1）
//   i2sMic.setPins(I2S_SCK_MIC, I2S_WS_MIC, -1, I2S_SD_MIC, -1);
//   // 开启 I2S，模式：标准模式，采样率、位宽，立体声（2通道）
//   if (!i2sMic.begin(I2S_MODE_STD, SAMPLE_RATE, BITS, (i2s_slot_mode_t)CHANNEL_STEREO)) {
//     Serial.println("麦克风I2S初始化失败！");
//     return;
//   }
//   // 配置 RX 转换，将16bit立体声转换为单声道（默认提取左声道）
//   if (!i2sMic.configureRX(SAMPLE_RATE, BITS, (i2s_slot_mode_t)CHANNEL_STEREO, I2S_RX_TRANSFORM_16_STEREO_TO_MONO)) {
//     Serial.println("麦克风配置RX转换失败！");
//     return;
//   }

//   Serial.println("初始化I2S喇叭（播放右声道）...");
//   // 配置喇叭：播放使用立体声模式
//   i2sSpeaker.setPins(I2S_SPK_BCLK, I2S_LRC_SPK, I2S_DOUT_SPK, -1, -1);
//   if (!i2sSpeaker.begin(I2S_MODE_STD, SAMPLE_RATE, BITS, (i2s_slot_mode_t)CHANNEL_STEREO)) {
//     Serial.println("喇叭I2S初始化失败！");
//     return;
//   }

//   // 分配录音缓冲区，注意：原始录音为立体声，数据量为 REC_BUFFER_SIZE 字节，
//   // 但经过 RX 转换后只保留左声道，数据量会减少一半
 
//   audioBuffer =(uint8_t *)heap_caps_malloc(REC_BUFFER_SIZE / 2, MALLOC_CAP_SPIRAM);
//   // audioBuffer = (uint8_t *)malloc(REC_BUFFER_SIZE / 2);
//   if (audioBuffer == nullptr) {
//     Serial.println("录音内存分配失败！");
//     return;
//   }

//   Serial.println("开始录音（仅左声道）...");
//   // 读取录音数据，实际返回的是单声道数据，字节数 = REC_BUFFER_SIZE/2
//   size_t bytesRead = i2sMic.readBytes((char *)audioBuffer, REC_BUFFER_SIZE / 2);
//   Serial.print("录音字节数: ");
//   Serial.println(bytesRead);

//   // 计算单声道采样数量（每个采样2字节）
//   size_t monoSampleCount = bytesRead / 2;

//   // 对录音数据进行声音放大和滤波处理
//   processAudio((int16_t *)audioBuffer, monoSampleCount, GAIN, ALPHA);

//   // 将处理后的单声道数据转换为立体声格式（仅右声道有效）
//   // 每个单声道采样（16bit）转换为一对16bit数据：左声道置0，右声道为处理后的录音数据
//   size_t playBufferSize = monoSampleCount * 2 * 2;  // 每个采样转换为 2 通道，每通道 2 字节
//   // 打印详细的内存信息
//   Serial.printf("请求分配内存大小: %d bytes\n", playBufferSize);
//   Serial.printf("SPIRAM可用总空间: %d bytes\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
//   Serial.printf("SPIRAM最大连续块: %d bytes\n", heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));

//   // 尝试分配内存
//   playBuffer = (uint8_t *)heap_caps_malloc(playBufferSize, MALLOC_CAP_SPIRAM);
//   if (playBuffer == nullptr) {
//       heap_caps_free(audioBuffer);
//       Serial.println("播放内存分配失败！");
      
//       // 打印内存分配失败后的状态
//       Serial.printf("分配失败后SPIRAM可用空间: %d bytes\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
//       Serial.printf("分配失败后最大连续块: %d bytes\n", heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));
//       return;
//   }
//   int16_t *monoSamples = (int16_t *)audioBuffer;
//   int16_t *stereoSamples = (int16_t *)playBuffer;
//   for (size_t i = 0; i < monoSampleCount; i++) {
//     stereoSamples[2 * i] = 0;                   // 左声道静音
//     stereoSamples[2 * i + 1] = monoSamples[i];  // 右声道填入处理后的录音数据
//   }

//   Serial.println("开始播放（仅右声道）...");
//   size_t bytesWritten = i2sSpeaker.write(playBuffer, playBufferSize);
//   Serial.print("播放字节数: ");
//   Serial.println(bytesWritten);

//   heap_caps_free(audioBuffer);
//   heap_caps_free(playBuffer);
//   // free(audioBuffer);
//   // free(playBuffer);
//   i2sMic.end();
//   i2sSpeaker.end();
// }

/* 麦克风测试 */
void microPhone_test() {
  Serial.println("mico_test");
  Wire.beginTransmission(0x30);            // 7 位地址 0x30
  Wire.write(0x17);                        // 发送命令 0x10
  uint8_t error = Wire.endTransmission();  // 结束传输并返回状态

  if (error == 0) {
    Serial.println("命令 0x70 发送成功");
  } else {
    Serial.print("命令发送错误，错误代码：");
    Serial.println(error);
  }

  Mic_init();
  Serial.println("mico_test");
  Wire.beginTransmission(0x30);    // 7 位地址 0x30
  Wire.write(0x18);                // 发送命令 0x10
  error = Wire.endTransmission();  // 结束传输并返回状态

  if (error == 0) {
    Serial.println("命令 0x18 发送成功");
  } else {
    Serial.print("命令发送错误，错误代码：");
    Serial.println(error);
  }
  while (1) {
    if (Serial.available()) {
      char serialData = Serial.read();
      if (serialData == 'v') {

        Serial.println("EXIT Mico test");
        break;
      }
    }
  }
}

//测试程序任务
void test_task() {
  char command[64] = { 0 };  //串口收到的命令
  char count = 0;            //命令的长度
  while (Serial.available()) {
    val = Serial.read();
    command[count] = val;
    count++;
  }
  switch (command[0]) {
    case 'T':
      Serial.println("Entry test");
      test_flag = true;
      show_test(800, 480, 300, 230, "Test Program");
      break;
    case 'I':
      Serial.println("Exit test");
      test_flag = false;
      esp_restart();  //退出测试重启
      break;
    case 'F':
      if (test_flag == true) {
        Serial.println("Display RED");
        gfx.fillScreen(TFT_RED);
      }
      break;
    case 'f':
      if (test_flag == true) {
        Serial.println("Exit test");
        gfx.fillScreen(TFT_BLACK);
      }
      break;
    case 'H':
      if (test_flag == true) {
        Serial.println("Display GREEN");
        gfx.fillScreen(TFT_GREEN);
      }
      break;
    case 'h':
      if (test_flag == true) {
        Serial.println("Exit test");
        gfx.fillScreen(TFT_BLACK);
      }
      break;
    case 'J':
      if (test_flag == true) {
        Serial.println("Display BLUE");
        gfx.fillScreen(TFT_BLUE);
      }
      break;
    case 'j':
      if (test_flag == true) {
        Serial.println("Exit test");
        gfx.fillScreen(TFT_BLACK);
      }
      break;
    case 'K':
      if (test_flag == true) {
        Serial.println("Display WHITE");
        gfx.fillScreen(TFT_WHITE);
      }
      break;
    case 'k':
      if (test_flag == true) {
        Serial.println("Exit test");
        gfx.fillScreen(TFT_BLACK);
      }
      break;
    case 'D':
      if (test_flag == true) {
        Serial.println("Display GRAY");
        gfx.fillScreen(TFT_DARKGREY);  // 0x8410
      }
      break;
    case 'd':
      if (test_flag == true) {
        Serial.println("Exit test");
        gfx.fillScreen(TFT_BLACK);
      }
      break;
    case 'U':
      if (test_flag == true) {
        Serial.println("Display test");
        lcd_test();
      }
      break;
    case 'u':
      if (test_flag == true) {
        Serial.println("Exit display test");
        gfx.fillScreen(TFT_BLACK);
      }
      break;
    case 'M':
      if (test_flag == true) {
        Serial.println("Touch test");
        show_test(800, 480, 300, 230, "Touch test");
        touch_test();
      }
      break;
    case 'C':
      if (test_flag == true) {
        Serial.println("RTC test");
        show_test(800, 480, 300, 230, "RTC test");
        get_bm8563_time();
      }
      break;
    case 'c':
      if (test_flag == true) {
        Serial.println("Exit RTC test");
        gfx.fillScreen(TFT_BLACK);
      }
      break;
    case 'P':
      if (test_flag == true) {
        Serial.println("GPIO test out high");
        show_test(800, 480, 260, 230, "GPIO test out high");
        //GPIO测试
        pinMode(19, OUTPUT);
        pinMode(20, OUTPUT);
        delay(10);
        digitalWrite(19, HIGH);
        digitalWrite(20, HIGH);
      }
      break;
    case 'p':
      if (test_flag == true) {
        Serial.println("Exit gpio test out low");
        gfx.fillScreen(TFT_BLACK);
        digitalWrite(19, LOW);
        digitalWrite(20, LOW);
      }
      break;
    case 'S':
      if (test_flag == true) {
        Serial.println("SD test");
        show_test(800, 480, 300, 230, "SD test");
        SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
        SD_init();
      }
      break;
    case 's':
      if (test_flag == true) {
        Serial.println("Exit SD test");
        gfx.fillScreen(TFT_BLACK);
        SD.end();
      }
      break;
    case 'Q':
      if (test_flag == true) {
        Serial.println("WIFI test");
        show_test(800, 480, 300, 230, "WIFI test");
        delay(1000);
        wifi_test();
        gfx.fillScreen(TFT_BLACK);
      }
      break;
    case 'B':
      if (test_flag == true) {
        Serial.println("BLE test");
        show_test(800, 480, 300, 230, "BLE test");
        ble_test();
      }
      break;
    case 'L':
      if (test_flag == true) {
        Serial.println("IIC OLE test");
        Wire.begin(15, 16);
        show_test(800, 480, 300, 230, "IIC OLE test");
        oled_test();
      }
      break;
    case 'l':
      if (test_flag == true) {
        Serial.println("Exit IIC OLE test");
        gfx.fillScreen(TFT_BLACK);
        oled_stop();
      }
      break;
    case 'Z':
      if (test_flag == true) {
        Serial.println("Buzzer test");
        show_test(800, 480, 300, 230, "Buzzer test");
        buzzer_test();
      }
      break;
    case 'z':
      if (test_flag == true) {
        Serial.println("Exit Buzzer test");
        gfx.fillScreen(TFT_BLACK);
        buzzer_stop();
      }
      break;
    case 'N':
      if (test_flag == true) {
        Serial.println("auto test");
        show_test(800, 480, 300, 230, "auto test");
        auto_test();
      }
      break;
    case 'n':
      if (test_flag == true) {
        Serial.println("Exit auto test");
        gfx.fillScreen(TFT_BLACK);
      }
      break;

    case 'X':
      if (test_flag == true) {
        Serial.println("SPI test");
        show_test(800, 480, 300, 230, "SPI test");

        gfx.fillScreen(TFT_WHITE);
        Serial.printf("[%d]-- turn white\n", __LINE__);

        // spi_test();
        lora_test();
      }
      break;
    case 'x':
      if (test_flag == true) {
        Serial.println("Exit SPI test");
        gfx.fillScreen(TFT_BLACK);
      }
      break;

    case 'G':
      if (test_flag == true) {
        Serial.println("Pressure test");
        show_test(800, 480, 300, 230, "Pressure test");
        int lcd_cont = 0;
        // 开始蓝牙服务
        pService->start();
        // 开始广播
        pServer->getAdvertising()->start();
        if (!WiFi.softAP("CrowPanel-Advance-HMI", "")) {
          return;
        }
        bool out = false;
        while (1) {
          char command[64] = { 0 };  //串口收到的命令
          char count = 0;            //命令的长度
          while (Serial.available()) {
            val = Serial.read();
            command[count] = val;
            count++;
          }
          if (val == 'g') {
            Serial.println("Exit pressure test");
            gfx.fillScreen(TFT_BLACK);
            pServer->getAdvertising()->stop();  //停止广播
            pService->stop();

            digitalWrite(19, LOW);
            digitalWrite(20, LOW);
            gfx.fillScreen(TFT_BLACK);
            buzzer_stop();
            return;
          }

          if (out == false) {
            digitalWrite(19, HIGH);
            digitalWrite(20, HIGH);
            buzzer_test();
            out = true;
            gfx.fillScreen(TFT_WHITE);
          } else {
            digitalWrite(19, LOW);
            digitalWrite(20, LOW);
            buzzer_stop();
            out = false;
            gfx.fillScreen(TFT_BLACK);
          }
          delay(500);
        }
      }
      break;

    /* 喇叭测试 */
    case 'W':
      if (test_flag == true) {
        Serial.println("[INFO] Speaker test!");
        show_test(800, 480, 300, 230, "Speaker test");
        speaker_test();
      }
      break;

    case 'w':
      if (test_flag == true) {
        Serial.println("[INFO] Exit Speaker test!");
        gfx.fillScreen(TFT_BLACK);
      }
      break;

    case 'V':
      if (test_flag == true) {
        Serial.println("[INFO] Microphone test!");
        show_test(800, 480, 300, 230, "Microphone test");

        gfx.fillScreen(TFT_BLACK);
        Serial.printf("[%d]-- turn white\n", __LINE__);

        microPhone_test();
      }
      break;

    case 'v':
      if (test_flag == true) {
        Serial.println("[INFO] Exit Microphone test!");
        gfx.fillScreen(TFT_BLACK);
      }
      break;
    case 'O':
      if (test_flag == true) {
        Serial.println("[INFO] PWM test!");
        gfx.fillScreen(TFT_WHITE);
        PWM();
        Serial.println("[INFO] Exit PWM test!");
      }
      break;
    default:
      break;
  }
}
void PWM() {
  char command[64] = { 0 };  //串口收到的命令
  char count = 0;            //命令的长度
  int cmd;
  while (1) {
    while (Serial.available()) {
      cmd = Serial.read();
      command[0] = cmd;
      // count++;
      Serial.println("接收指令：");
      Serial.println(command[0]);
      if (command[0] == '0') {
        sendI2CCommand(0x0);
      } else if (command[0] == '1') {
        sendI2CCommand(0x05);
      } else if (command[0] == '2') {
        sendI2CCommand(0x06);
      } else if (command[0] == '3') {
        sendI2CCommand(0x07);
      } else if (command[0] == '4') {
        sendI2CCommand(0x08);
      } else if (command[0] == '5') {
        sendI2CCommand(0x09);
      } else if (command[0] == '6') {
        sendI2CCommand(0x10);
      } else if (command[0] == '7') {
        sendI2CCommand(0x10);
        return;
      }
    }
  }
}
// 封装函数，用于发送 I2C 命令
void sendI2CCommand(uint8_t command) {
  uint8_t error;
  // 开始向指定地址发送命令
  Wire.beginTransmission(0x30);
  // 发送命令
  Wire.write(command);
  // 结束传输并返回状态
  error = Wire.endTransmission();

  if (error == 0) {
    Serial.print("命令 0x");
    Serial.print(command, HEX);
    Serial.println(" 发送成功");
  } else {
    Serial.print("命令发送错误，错误代码：");
    Serial.println(error);
  }
}
void setup() {

  pinMode(19, OUTPUT);
  pinMode(20, OUTPUT);
  //GT911 上电时序  ---> 选用 0x5D
  pinMode(1, OUTPUT);
  digitalWrite(1, LOW);
  //ioex.output(2, TCA9534::Level::L);
  //ioex.output(2, TCA9534::Level::H);
  delay(120);
  pinMode(1, INPUT);
  /*end*/

  // Create the BLE Device
  BLEDevice::init("CrowPanel-Advance-HMI-7.0");
  // 创建蓝牙服务器
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  // 创建广播服务的UUID
  pService = pServer->createService(SERVICE_UUID);
  //  //创建广播服务的UUID
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic->setCallbacks(new MyCallbacks());
  //  pServer->getAdvertising()->start();

  Serial.begin(115200);  // Init Display
  first_flag = 0;

  Wire.begin(15, 16);
  delay(50);
  while (1) {
    if (i2cScanForAddress(0x30) && i2cScanForAddress(0x5D)) {
      Serial.print("The microcontroller is detected: address 0x");
      Serial.println(0x30, HEX);
      Serial.print("The microcontroller is detected: address 0x");
      Serial.println(0x5D, HEX);


      break;
    } else {
      Serial.print("No microcontroller was detected: address 0x");
      Serial.println(0x30, HEX);
      Serial.print("No microcontroller was detected: address 0x");
      Serial.println(0x5D, HEX);
      //Prevent the microcontroller did not start to adjust the bright screen
      sendI2CCommand(0x19);
      pinMode(1, OUTPUT);
      digitalWrite(1, LOW);
      //ioex.output(2, TCA9534::Level::L);
      //ioex.output(2, TCA9534::Level::H);
      delay(120);
      pinMode(1, INPUT);

      delay(100);
    }
  }




  // ioex.attach(Wire);
  // ioex.setDeviceAddress(0x18);
  // ioex.config(1, TCA9534::Config::OUT);
  // ioex.config(2, TCA9534::Config::OUT);
  // ioex.config(3, TCA9534::Config::OUT);
  // ioex.config(4, TCA9534::Config::OUT);

  // /* Backlight on */
  // ioex.output(1, TCA9534::Level::H);

  // /* unmute */
  // ioex.output(3, TCA9534::Level::L);
  // ioex.output(4, TCA9534::Level::H);

  // Begin sending commands to address 0x18 to enable mute
  sendI2CCommand(0x18);


  // Init Display
  gfx.init();
  gfx.initDMA();
  gfx.startWrite();
  gfx.fillScreen(TFT_BLACK);


  lv_init();
  size_t buffer_size = sizeof(lv_color_t) * LCD_H_RES * LCD_V_RES;
  buf = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
  buf1 = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);

  lv_disp_draw_buf_init(&draw_buf, buf, buf1, LCD_H_RES * LCD_V_RES);

  //初始化显示
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  //将以下行更改为您的显示分辨率
  disp_drv.hor_res = LCD_H_RES;
  disp_drv.ver_res = LCD_V_RES;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  //初始化输入设备驱动程序
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  delay(100);

  // 开始向地址 0x30 发送命令 0x10
  sendI2CCommand(0x10);

  ui_init();  //开机UI界面
  while (1) {
    if (goto_widget_flag == 1)  //进入widget
    {
      if (ticker1.active() == true) {
        ticker1.detach();
      }
      goto_widget_flag = 0;
      delay(300);
      break;
    }

    if (goto_widget_flag == 3)  //进入触摸界面，先把进度条线程关闭
    {
      bar_flag = 0;  //停止进度条标志
      if (ticker1.active() == true) {
        ticker1.detach();
      }
      if (first_flag == 0 || first_flag == 1) {
        label_xy();
        first_flag = 2;
      }
      if (zero_clean == 1) {
        touch_x = 0;
        touch_y = 0;
        zero_clean = 0;
      }
      lv_label_set_text(ui_Label, "Touch Adjust:");
      lv_label_set_text_fmt(ui_Label3, "%d  %d", touch_x, touch_y);  //显示触摸信息
    }

    if (goto_widget_flag == 4)  //触摸界面返回到Menu界面,使进度条清零重启
    {
      val = 100;
      delay(100);
      ticker1.attach_ms(35, callback1);  //每20ms调用callback1
      goto_widget_flag = 0;
    }

    if (goto_widget_flag == 5)  //触发校准信号
    {
      Serial.println("Setup touch_calibrate");
      lv_scr_load_anim(ui_touch_calibrate, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
      lv_timer_handler();
      lv_timer_handler();
      delay(100);
      touch_calibrate();  //触摸校准
      lv_scr_load_anim(ui_TOUCH, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
      lv_timer_handler();
      goto_widget_flag = 3;  //进入触摸界面标志
      touch_x = 0;
      touch_y = 0;
    }

    if (bar_flag == 6)  //刚开机进入Menu界面时运行进度条一次，之后就不再运行
    {
      if (first_flag == 0) {
        lv_example_bar();
        ticker1.attach_ms(35, callback1);  //每20ms调用callback1
        first_flag = 1;
      }
    }

    lv_timer_handler();
  }

  gfx.fillScreen(TFT_BLACK);
  lv_demo_widgets();  //主UI界面

  Serial.println("Setup done");
  lv_timer_handler();
 
  test_flag = false;
}

void loop() {
  //测试任务
  test_task();
  if (test_flag == false) {
    lv_timer_handler(); /* let the GUI do its work */
    delay(1);
  }
}

/* 自动测试 */
void auto_test() {
  char command[64] = { 0 };  //串口收到的命令
  char count = 0;            //命令的长度
  uint8_t i = 0;
  uint8_t touch_cont = 0;
  bool touch_flag = false;
  int touch_ = 0;
  while (1) {
    while (Serial.available()) {
      val = Serial.read();
      command[count] = val;
      count++;
    }
    if (command[0] == 'n') {
      Serial.println("auto test break");
      return;
    }

    switch (i) {
      case 0:
        Serial.println("Display test");
        if (lcd_test() == 1) {
          Serial.println("Exit auto test");
          return;
        }
        delay(1000);
        break;
      case 1:
        Serial.println("Touch test");
        show_test(800, 480, 300, 230, "Touch test");
        while (1) {
          while (Serial.available()) {
            val = Serial.read();
            command[count] = val;
            count++;
          }
          if (command[0] == 'n') {
            Serial.println("auto test break");
            return;
          }
          bool touched = gfx.getTouch(&touch_x, &touch_y);
          if (touched) {
            touch_++;
            if (touch_ > 2) {
              touch_ = 2;
              touch_flag = true;
              Serial.print("Data x :");
              Serial.println(touch_x);
              Serial.print("Data y :");
              Serial.println(touch_y);
            }
          }
          if (touch_flag) {
            delay(100);
            touch_cont++;
            if (touch_cont > 50) {
              touch_cont = 0;
              delay(2000);
              Serial.println("EXIT Touch test");
              delay(1000);
              break;
            }
          }
        }
        break;
      case 2:
        Serial.println("RTC test");
        show_test(800, 480, 300, 230, "RTC test");
        get_bm8563_time();
        delay(4000);
        Serial.println("Exit RTC test");
        delay(1000);
        break;
      case 3:
        Serial.println("GPIO test out high");
        show_test(800, 480, 260, 230, "GPIO test out high");
        digitalWrite(19, HIGH);
        digitalWrite(20, HIGH);
        delay(4000);
        Serial.println("Exit gpio test out low");
        digitalWrite(19, LOW);
        digitalWrite(20, LOW);
        delay(1000);
        break;
      case 4:
        Serial.println("SD test");
        show_test(800, 480, 300, 230, "SD test");
        SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
        SD_init();
        delay(4000);
        Serial.println("Exit SD test");
        SD.end();
        delay(1000);
        break;
      case 5:
        Serial.println("IIC OLE test");
        show_test(800, 480, 300, 230, "IIC OLE test");
        oled_test();
        delay(4000);
        Serial.println("Exit IIC OLE test");
        oled_stop();
        delay(1000);
        break;
      case 6:
        Serial.println("Buzzer test");
        show_test(800, 480, 300, 230, "Buzzer test");
        buzzer_test();
        delay(4000);
        Serial.println("Exit Buzzer test");
        buzzer_stop();
        delay(1000);
        break;
      case 7:
        Serial.println("SPI test");
        show_test(800, 480, 300, 230, "SPI test");
        // spi_test();
        lora_test();
        delay(4000);
        Serial.println("Exit SPI test");
        delay(1000);
        Serial.println("Exit auto finish");
        gfx.fillScreen(TFT_BLACK);
        return;
    }
    i++;
  }
}
