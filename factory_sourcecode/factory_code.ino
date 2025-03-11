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
#include "BLEDevice.h"              //BLE Driver Library
#include "BLEServer.h"              //BLE Bluetooth server library
#include "BLEUtils.h"               //BLE Utilities Library
#include "BLE2902.h"                //Feature Add Descriptor Library
#include "Arduino.h"
#include "WiFiMulti.h"
#include "Audio.h"
#include "ESP_I2S.h"

// Used to set I2S related configuration parameters
#define I2S_SAMPLE_RATE 44100                        // Sample rate, can be adjusted according to the actual support of the microphone 44100
#define I2S_DATA_BIT_WIDTH I2S_DATA_BIT_WIDTH_16BIT  // Bit width of the data, set to 16 bits.
#define I2S_SLOT_MODE I2S_SLOT_MODE_MONO             // Channel mode, set to mono, can be changed to stereo etc. as needed.

// The microphone is connected to the I2S pin number of the ESP32S3, modified according to the actual hardware connection
#define I2S_BCLK_PIN 19
#define I2S_WS_PIN 2
#define I2S_DIN_PIN 20

#define I2S_DOUT 4
#define I2S_BCLK 5
#define I2S_LRC 6
I2SClass i2s;



BLECharacteristic *pCharacteristic;
BLEServer *pServer;
BLEService *pService;
bool deviceConnected = false;
char BLEbuf[32] = {0};

#define OLED_RESET     -1
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* RTC BM8563 I2C port */
#define BM8563_I2C_SDA 15
#define BM8563_I2C_SCL 16
I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire1);

//2401
#include <nRF24L01.h>
#include <RF24.h>
#define CE_PIN 20   //txrx reuse
#define CSN_PIN 19
// instantiate an object for the nRF24L01 transceiver
RF24 rf_radio(CE_PIN, CSN_PIN);

/* audio module */
WiFiMulti wifiMulti;
Audio audio;
bool audioIsInited = false;

/* extensions io */
#include <TCA9534.h>
TCA9534 ioex;

#define HSPI_MISO  4
#define HSPI_MOSI  6
#define HSPI_SCLK  5
#define HSPI_SS    19

#define SD_MOSI 6
#define SD_MISO 4
#define SD_SCK 5
#define SD_CS 0 //The chip selector pin is not connected to IO

#define I2S_DOUT 4
#define I2S_BCLK 5
#define I2S_LRC 6

#define LOG_DEBUG() do{           \
  Serial.print("FUNCTION[");      \
  Serial.print(__FUNCTION__);     \
  Serial.print("]\tLINE[");       \
  Serial.print(__LINE__);         \
  Serial.println("]");            \
}while(0)

SPIClass* hspi = nullptr;

const char* ntpServer = "time.cloudflare.com";

LGFX gfx;

void show_test(int lcd_w, int lcd_h, int x, int y, const char * text) {
  gfx.fillScreen(TFT_BLACK);
  gfx.setTextSize(3);
  gfx.setTextColor(TFT_RED);
  gfx.setCursor(x, y);
  gfx.print(text); // Show text
}

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("------> BLE connect .");
      show_test(800, 480, 300, 230, "BLE connect");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("------> BLE disconnect .");
      show_test(800, 480, 300, 230, "BLE disconnect");
      pServer->startAdvertising(); // restart advertising
      Serial.println("start advertising");
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
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
        }
        else if (rxValue.indexOf("B") != -1) {
          Serial.print("Rx B!");
        }
        Serial.println();
      }
    }
};

bool test_flag; //Test Program Judgment

#include <Ticker.h>          //Call the ticker. H Library
Ticker ticker1;
Ticker ticker_getRTC;

//UI
#include "ui.h"
static int first_flag = 0;
extern int zero_clean;
extern int goto_widget_flag;
extern int bar_flag;
extern lv_obj_t * ui_MENU;
extern lv_obj_t * ui_TOUCH;
extern lv_obj_t * ui_JIAOZHUN;
extern lv_obj_t * ui_Label2;
static lv_obj_t * ui_Label;//TOUCH interfaces label
static lv_obj_t * ui_Label3;//TOUCH interfaces label3
static lv_obj_t * ui_Labe2;//Menu interface progress bar label
static lv_obj_t * bar;//Menu interface progress bar

/* Change to your screen resolution */
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;
static lv_color_t *buf1;

uint16_t touch_x, touch_y;

//Display Refresh
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  if (gfx.getStartCount() > 0) {
    gfx.endWrite();
  }
  gfx.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::rgb565_t *)&color_p->full);

  lv_disp_flush_ready(disp);  //
}

//Read Touch
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
  data->state = LV_INDEV_STATE_REL;
  bool touched = gfx.getTouch( &touch_x, &touch_y );
  if (touched)
  {
    data->state = LV_INDEV_STATE_PR;

    //Setting the coordinates
    data->point.x = touch_x;
    data->point.y = touch_y;
  }
}

static int val = 100;
void callback1()  //Callback function
{
  if (bar_flag == 6)
  {
    if (val > 1)
    {
      val--;
      lv_bar_set_value(bar, val, LV_ANIM_OFF);
      lv_label_set_text_fmt(ui_Labe2, "%d %%", val);
    }
    else
    {
      lv_obj_clear_flag(ui_touch, LV_OBJ_FLAG_CLICKABLE);
      lv_label_set_text(ui_Labe2, "Loading");
      delay(150);
      val = 100;
      bar_flag = 0; //Stop progress bar sign
      goto_widget_flag = 1; //Enter the widget logo

    }
  }
}

void calibrateTouch(uint16_t *parameters, uint32_t color_fg, uint32_t color_bg, uint8_t size) {
  int16_t values[] = {0, 0, 0, 0, 0, 0, 0, 0};
  uint16_t x_tmp, y_tmp;
  uint16_t _width = 800;
  uint16_t _height = 480;

  for (uint8_t i = 0; i < 4; i++) {
    gfx.fillRect(0, 0, size + 1, size + 1, color_bg);
    gfx.fillRect(0, _height - size - 1, size + 1, size + 1, color_bg);
    gfx.fillRect(_width - size - 1, 0, size + 1, size + 1, color_bg);
    gfx.fillRect(_width - size - 1, _height - size - 1, size + 1, size + 1, color_bg);

    switch (i) {
      case 0: // up left
        gfx.fillRect(0, 0, size + 1, size + 1, color_fg);
        break;
      case 1: // bot left
        gfx.fillRect(0, _height - size - 1, size + 1, size + 1, color_fg);
        break;
      case 2: // up right
        gfx.fillRect(_width - size - 1, 0, size + 1, size + 1, color_fg);
        break;
      case 3: // bot right
        gfx.fillRect(_width - size - 1, _height - size - 1, size + 1, size + 1, color_fg);
        break;
    }

    // user has to get the chance to release
    if (i > 0) delay(1000);

    for (uint8_t j = 0; j < 8; j++) {
      while (1)
      {
        bool touched = gfx.getTouch( &touch_x, &touch_y );
        if (touched)
        {
          Serial.print( "Data x :" );
          Serial.println( touch_x );
          Serial.print( "Data y :" );
          Serial.println( touch_y );
          break;
        }
      }
    }
  }
}

void touch_calibrate()//screen calibration
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;
  Serial.println("screen calibration");

  Serial.println("touch_calibrate ...");

  lv_timer_handler();
  calibrateTouch(calData, TFT_RED, TFT_BLACK, 17);
  Serial.println("calibrateTouch(calData, TFT_RED, TFT_BLACK, 15)");
  Serial.println(); Serial.println();
  Serial.println("//在setup()use code:");
  Serial.print("uint16_t calData[5] = ");
  Serial.print("{ ");

  for (uint8_t i = 0; i < 5; i++)
  {
    Serial.print(calData[i]);
    if (i < 4) Serial.print(", ");
  }

  Serial.println(" };");
}

//Touch Label Controls
void label_xy()
{
  ui_Label = lv_label_create(ui_TOUCH);
  lv_obj_enable_style_refresh(true);
  lv_obj_set_width(ui_Label, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_height(ui_Label, LV_SIZE_CONTENT);    /// 1
  lv_obj_set_x(ui_Label, -55);
  lv_obj_set_y(ui_Label, -40);
  lv_obj_set_align(ui_Label, LV_ALIGN_CENTER);
  lv_obj_set_style_text_color(ui_Label, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(ui_Label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui_Label, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

  ui_Label3 = lv_label_create(ui_TOUCH);
  lv_obj_enable_style_refresh(true);
  lv_obj_set_width(ui_Label3, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_height(ui_Label3, LV_SIZE_CONTENT);    /// 1
  lv_obj_set_x(ui_Label3, 85);
  lv_obj_set_y(ui_Label3, -40);
  lv_obj_set_align(ui_Label3, LV_ALIGN_CENTER);
  lv_obj_set_style_text_color(ui_Label3, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(ui_Label3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui_Label3, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
}


//Progress bar control
void lv_example_bar(void)
{
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
  ui_Labe2 = lv_label_create(bar);
  lv_obj_set_style_text_color(ui_Labe2, lv_color_hex(0x09BEFB), LV_STATE_DEFAULT);
  lv_label_set_text(ui_Labe2, "0%");
  lv_obj_center(ui_Labe2);
}

void get_ntp_BM8563_Test()
{
  // Set ntp time to local
  configTime(8 * 3600, 0, ntpServer); //China time
  printLocalTime();
}

void printLocalTime()
{ 
   // Init I2C
  Wire1.begin(BM8563_I2C_SDA, BM8563_I2C_SCL);
  
  // Init RTC
  rtc.begin();
  
  struct tm timeInfo;
  if (!getLocalTime(&timeInfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  // Format and print time
  Serial.print("NTP Time:  ");
  Serial.println(&timeInfo, "%A, %B %d %Y %H:%M:%S");

  // Set RTC time
  I2C_BM8563_TimeTypeDef timeStruct;
  timeStruct.hours   = timeInfo.tm_hour;
  timeStruct.minutes = timeInfo.tm_min;
  timeStruct.seconds = timeInfo.tm_sec;
  rtc.setTime(&timeStruct);

  // Set RTC Date
  I2C_BM8563_DateTypeDef dateStruct;
  dateStruct.weekDay = timeInfo.tm_wday;
  dateStruct.month   = timeInfo.tm_mon + 1;
  dateStruct.date    = timeInfo.tm_mday;
  dateStruct.year    = timeInfo.tm_year + 1900;
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

void get_bm8563_time()
{
  Wire1.begin(BM8563_I2C_SDA, BM8563_I2C_SCL);
  
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
                timeStruct.seconds
               );
}

void buzzer_test()
{
  analogWrite(8, 249);
}

void buzzer_stop()
{
  analogWrite(8, 0);
}

void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(25, 25);            // Start at top-left corner
  display.println(F("ELECROW"));
  display.display();
  //  delay(2000);
}

void oled_test()
{
  //Initializing the display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  testdrawstyles();
}

void oled_stop()
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.display();
}

void wifi_test()
{
  char command[64] = {0};   
  char count = 0;           
  bool flag = false;
  bool timeout_flag = false;
  while (1)
  {
    while (Serial.available())
    {
      String ssid = Serial.readStringUntil(','); 
      String password = Serial.readStringUntil('\n'); 
      
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
        if (timeout == 40)
        {
          Serial.println("Connection timeout exit");
          Serial.println("EXIT WIFI test");
          char command[64] = {0};   
          char count = 0;           
          return;
        }
      }
      if (timeout_flag == false)
      {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());

        show_test(800, 480, 300, 230, "WiFi connected");
        Serial.println("Update the RTC time");
        get_ntp_BM8563_Test(); //get NTP time

        char command[64] = {0};   
        char count = 0;          
        flag = true;

      }
    }
    if (flag == true)
    {
      Serial.println("EXIT WIFI test");
      WiFi.disconnect();
      char command[64] = {0};   
      char count = 0;           
      break;
    }
    delay(10);
  }
}

void ble_test()
{
  char command[64] = {0};   
  char count = 0;          

  
  pService->start();
  
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");

  while (1)
  {
    while (Serial.available())
    {
      val = Serial.read();
      command[count] = val;
      count++;
    }
    if (command[0] == 'b')
    {
      Serial.println("EXIT ble test");
      gfx.fillScreen(BLACK);
      pServer->getAdvertising()->stop();
      pService->stop();
      break;
    }

    if (deviceConnected) {
      memset(BLEbuf, 0, 32);
      memcpy(BLEbuf, (char*)"Hello BLE APP!", 32);
      pCharacteristic->setValue(BLEbuf);

      pCharacteristic->notify(); // Send the value to the app!
      Serial.print("*** Sent Value: ");
      Serial.print(BLEbuf);
      Serial.println(" ***");
    }
    delay(1000);
  }
}

int lcd_test()
{
  char command[64] = {0};   
  char count = 0;           
  int i = 0 ;
  while (1)
  {
    while (Serial.available())
    {
      val = Serial.read();
      command[count] = val;
      count++;
    }
    if (command[0] == 'n')
    {
      Serial.println("auto test break");
      gfx.fillScreen(TFT_BLACK);
      return 1;
    }

    switch (i)
    {
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
        gfx.fillScreen(TFT_DARKGREY); // 0x8410
        delay(2000);
        Serial.println("Exit display finish");
        gfx.fillScreen(TFT_BLACK);
        return 0;
    }

    i++;
  }
}

/* Lower computer touch test */
void touch_test()
{
  char command[64] = {0};   
  char count = 0;          
  uint16_t last_x = 0;
  uint16_t last_y = 0;
  while (1)
  {
    while (Serial.available())
    {
      val = Serial.read();
      command[count] = val;
      count++;
    }
    if (command[0] == 'm')
    {
      Serial.println("EXIT Touch test");
      gfx.fillScreen(TFT_BLACK);
      break;
    }
    bool touched = gfx.getTouch( &touch_x, &touch_y );
    // Serial.printf("x[%d]\ty[%d]\n", touch_x, touch_y);
    // Serial.printf("\n[INFO]--->touched is [%s]\n", touched ? "True" : "False");
    if ( touched && (last_x != touch_x || last_y != touch_y) ) {
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

//SD
void SD_init()
{
  hspi = new SPIClass(HSPI); // by default VSPI is used
  hspi->begin(SD_SCK, SD_MISO, SD_MOSI);
  if (!SD.begin(SD_CS, *hspi, 80000000))
  {
    Serial.println(F("ERROR: File system mount failed!"));
    hspi->end();
  }
  else
  {
    Serial.printf("SD Size: %lluMB \n", SD.cardSize() / (1024 * 1024));
    hspi->end();
  }
}


void spi_test()
{
  hspi = new SPIClass(HSPI); // by default VSPI is used
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
/* lora */
// ESP32 - SX126x pin configuration
int PIN_LORA_RESET = 19;  // LORA RESET
int PIN_LORA_DIO_1 = 20; // LORA DIO_1
int PIN_LORA_BUSY = 2;  // LORA SPI BUSY
int PIN_LORA_NSS = 0;  // LORA SPI CS
int PIN_LORA_SCLK = 5;  // LORA SPI CLK
int PIN_LORA_MISO = 4;  // LORA SPI MISO
int PIN_LORA_MOSI = 6;  // LORA SPI MOSI
int RADIO_TXEN = -1;   // LORA ANTENNA TX ENABLE
int RADIO_RXEN = -1;   // LORA ANTENNA RX ENABLE
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
/* lora  */
void lora_test() {
  /* lora配置 */
  hwConfig.CHIP_TYPE = SX1262_CHIP;     // Example uses an eByte E22 module with an SX1262
  hwConfig.PIN_LORA_RESET = PIN_LORA_RESET; // LORA RESET
  hwConfig.PIN_LORA_NSS = PIN_LORA_NSS;  // LORA SPI CS
  hwConfig.PIN_LORA_SCLK = PIN_LORA_SCLK;   // LORA SPI CLK
  hwConfig.PIN_LORA_MISO = PIN_LORA_MISO;   // LORA SPI MISO
  hwConfig.PIN_LORA_DIO_1 = PIN_LORA_DIO_1; // LORA DIO_1
  hwConfig.PIN_LORA_BUSY = PIN_LORA_BUSY;   // LORA SPI BUSY
  hwConfig.PIN_LORA_MOSI = PIN_LORA_MOSI;   // LORA SPI MOSI
  hwConfig.RADIO_TXEN = RADIO_TXEN;     // LORA ANTENNA TX ENABLE
  hwConfig.RADIO_RXEN = RADIO_RXEN;     // LORA ANTENNA RX ENABLE
  hwConfig.USE_DIO2_ANT_SWITCH = true;    // Example uses an CircuitRocks Alora RFM1262 which uses DIO2 pins as antenna control
  hwConfig.USE_DIO3_TCXO = true;        // Example uses an CircuitRocks Alora RFM1262 which uses DIO3 to control oscillator voltage
  hwConfig.USE_DIO3_ANT_SWITCH = false;  // Only Insight ISP4520 module uses DIO3 as antenna control

  Serial.println("=====================================");
  Serial.println("lora1262 ID test");
  Serial.println("=====================================");

  Serial.println("MCU Espressif ESP32");
  uint8_t deviceId[8] = {0};

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
  if(ret == 0) {
    Serial.println("[INFO] Successful initialisation of the lora also means that it is able to communicate successfully");
  } else if(ret == 1) {
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
  //          LORA_SPREADING_FACTOR, LORA_CODINGRATE,
  //          LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
  //          true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);

  // // Set Radio RX configuration
  // Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
  //          LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
  //          LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
  //          0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

  // // Start LoRa
  // Serial.println("Starting Radio.Rx");
  // Radio.Rx(RX_TIMEOUT_VALUE);
}

//  speaker Test
void speaker_test() {
  
  WiFi.mode(WIFI_STA);
  char command[64] = {0};   
  char count = 0;           
  bool flag = false;
  bool timeout_flag = false;
  bool readyToPlay = false;
  delay(1000);
  while (1) {
    while (Serial.available()) {
      String ssid = Serial.readStringUntil(',');      
      String password = Serial.readStringUntil(','); 
      String urlStr = Serial.readStringUntil('\n');   
      
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
          char command[64] = {0};  
          char count = 0;           
          timeout_flag = true;
          return;
        }
      }
      if (timeout_flag == false) {
        Serial.println("");
        Serial.println("[INFO] WiFi connected");
        
        if(audioIsInited == false) {
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
        
        readyToPlay = true;
        char command[64] = {0};  
        char count = 0;          
      }
    }

    if(readyToPlay) {
     
      ioex.output(4, TCA9534::Level::L);
      while(1) {
        audio.loop();
        if(Serial.available()) {
          audio.stopSong();
          String r = Serial.readString();
          r.trim();
          if (r.length() > 5){
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
      ioex.output(4, TCA9534::Level::H); 
      gfx.fillScreen(TFT_BLACK);
      break;
    }
    delay(10);
  }
}

/*microPhone_test */
void microPhone_test() {
 Serial.println("mico_test");


  
  i2s.setPins(I2S_BCLK_PIN, I2S_WS_PIN, -1, I2S_DIN_PIN); 

  
  if (!i2s.begin(I2S_MODE_STD, I2S_SAMPLE_RATE, I2S_DATA_BIT_WIDTH, I2S_SLOT_MODE)) {
    Serial.println("I2S initialization failed!");
    while (1)
      ;  
  }
  while (1) {
    
    char buffer[64];
    size_t bytesRead = i2s.readBytes(buffer, sizeof(buffer));  
    if (bytesRead > 0) {
      Serial.print("Read ");
      Serial.print(bytesRead);
      Serial.println(" bytes of audio data:");
      
      if (bytesRead >= 2) {
        int16_t firstSlotSample = ((int16_t *)buffer)[0]; 
        Serial.print(abs(firstSlotSample));
        Serial.println(" ");
      } else {
        Serial.println("Not enough data for a complete sample in the first slot.");
      }
    } else {
      Serial.println("Error reading audio data from I2S");
    }


    delay(500);  

    if (Serial.available()) {
      char serialData = Serial.read();
      if (serialData == 'v') {
        i2s.end();
        Serial.println("EXIT Mico test");
        break;
      }
    }
  }
}

//test_task
void test_task()
{
  char command[64] = {0};   
  char count = 0;          
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
      esp_restart();  
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
        gfx.fillScreen(TFT_DARKGREY); // 0x8410
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
    case 'A':
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
        Serial.printf("[%d]-- turn white\n",__LINE__);

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
        
        pService->start();
        
        pServer->getAdvertising()->start();
        if (!WiFi.softAP("CrowPanel-Advance-HMI", "")) {
          return ;
        }
        bool out = false;
        while (1) {
          char command[64] = {0};  
          char count = 0;           
          while (Serial.available()) {
            val = Serial.read();
            command[count] = val;
            count++;
          }
          if (val == 'g') {
            Serial.println("Exit pressure test");
            gfx.fillScreen(TFT_BLACK);
            pServer->getAdvertising()->stop();
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

    
    case 'W':
      if(test_flag == true) {
        Serial.println("[INFO] Speaker test!");
        show_test(800, 480, 300, 230, "Speaker test");
        speaker_test();
      }
      break;

    case 'w':
      if(test_flag == true) {
        Serial.println("[INFO] Exit Speaker test!");
        gfx.fillScreen(TFT_BLACK);
      }
      break;

    case 'V':
      if(test_flag == true) {
        Serial.println("[INFO] Microphone test!");
        show_test(800, 480, 300, 230, "Microphone test");

        gfx.fillScreen(TFT_BLACK);
        Serial.printf("[%d]-- turn white\n",__LINE__);

        microPhone_test();
      }
      break;

    case 'v':
      if(test_flag == true) {
        Serial.println("[INFO] Exit Microphone test!");
        gfx.fillScreen(TFT_BLACK);
      }
      break;
        
    default:
      break;
  }
}


void setup()
{
  pinMode(8, OUTPUT); //buzzer
  pinMode(19, OUTPUT);
  pinMode(20, OUTPUT);

  // Create the BLE Device
  BLEDevice::init("CrowPanel-Advance-HMI-4.3");
 
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  pService = pServer->createService(SERVICE_UUID);
  
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic->setCallbacks(new MyCallbacks());
  //  pServer->getAdvertising()->start();

  Serial.begin(115200); // Init Display
  first_flag = 0;

  Wire.begin(15, 16);
  delay(50);

  ioex.attach(Wire);
  ioex.setDeviceAddress(0x18);
  ioex.config(1, TCA9534::Config::OUT);
  ioex.config(2, TCA9534::Config::OUT);
  ioex.config(3, TCA9534::Config::OUT);
  ioex.config(4, TCA9534::Config::OUT);

  
  ioex.output(1, TCA9534::Level::H);

  
  ioex.output(3, TCA9534::Level::L);
  ioex.output(4, TCA9534::Level::H);

  
  pinMode(1, OUTPUT);
  digitalWrite(1, LOW);
  ioex.output(2, TCA9534::Level::L);
  delay(20);
  ioex.output(2, TCA9534::Level::H);
  delay(100);
  pinMode(1, INPUT);
  /*end*/

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

  
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  
  disp_drv.hor_res = LCD_H_RES;
  disp_drv.ver_res = LCD_V_RES;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  delay(100);

  ui_init();
  while (1)
  {
    if (goto_widget_flag == 1)//进入widget
    {
      if (ticker1.active() == true)
      {
        ticker1.detach();
      }
      goto_widget_flag = 0;
      delay(300);
      break;
    }

    if (goto_widget_flag == 3)
    {
      bar_flag = 0;
      if (ticker1.active() == true)
      {
        ticker1.detach();
      }
      if (first_flag == 0 || first_flag == 1)
      {
        label_xy();
        first_flag = 2;
      }
      if (zero_clean == 1)
      {
        touch_x = 0;
        touch_y = 0;
        zero_clean = 0;
      }
      lv_label_set_text(ui_Label, "Touch Adjust:");
      lv_label_set_text_fmt(ui_Label3, "%d  %d", touch_x, touch_y); 
    }

    if (goto_widget_flag == 4)
    {
      val = 100;
      delay(100);
      ticker1.attach_ms(35, callback1);
      goto_widget_flag = 0;
    }

    if (goto_widget_flag == 5) 
    {
      Serial.println( "Setup touch_calibrate" );
      lv_scr_load_anim(ui_touch_calibrate, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
      lv_timer_handler();
      lv_timer_handler();
      delay(100);
      touch_calibrate();
      lv_scr_load_anim(ui_TOUCH, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
      lv_timer_handler();
      goto_widget_flag = 3;
      touch_x = 0;
      touch_y = 0;
    }

    if (bar_flag == 6)
    {
      if (first_flag == 0)
      {
        lv_example_bar();
        ticker1.attach_ms(35, callback1);
        first_flag = 1;
      }
    }

    lv_timer_handler();
  }

  gfx.fillScreen(TFT_BLACK);
  lv_demo_widgets();

  Serial.println( "Setup done" );

  test_flag = false;
}

void loop()
{
  
  test_task();
  if (test_flag == false)
  {
    lv_timer_handler(); /* let the GUI do its work */
    delay(1);
  }
}


void auto_test()
{
  char command[64] = {0};  
  char count = 0;          
  uint8_t i = 0;
  uint8_t touch_cont = 0;
  bool touch_flag = false;
  int touch_ = 0;
  while (1)
  {
    while (Serial.available())
    {
      val = Serial.read();
      command[count] = val;
      count++;
    }
    if (command[0] == 'n')
    {
      Serial.println("auto test break");
      return ;
    }

    switch (i)
    {
      case 0:
        Serial.println("Display test");
        if (lcd_test() == 1)
        {
          Serial.println("Exit auto test");
          return ;
        }
        delay(1000);
        break;
      case 1:
        Serial.println("Touch test");
        show_test(800, 480, 300, 230, "Touch test");
        while (1)
        {
          while (Serial.available())
          {
            val = Serial.read();
            command[count] = val;
            count++;
          }
          if (command[0] == 'n')
          {
            Serial.println("auto test break");
            return ;
          }
          bool touched = gfx.getTouch( &touch_x, &touch_y );
          if (touched)
          {
            touch_++;
            if (touch_ > 2)
            {
              touch_ = 2;
              touch_flag = true;
              Serial.print( "Data x :" );
              Serial.println( touch_x );
              Serial.print( "Data y :" );
              Serial.println( touch_y );
            }
          }
          if (touch_flag)
          {
            delay(100);
            touch_cont++;
            if (touch_cont > 50)
            {
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
        return ;
    }
    i++;
  }
}
