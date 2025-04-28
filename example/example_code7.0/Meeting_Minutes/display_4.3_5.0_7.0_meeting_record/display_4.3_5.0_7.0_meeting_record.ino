#include <WiFi.h>
#include <WebSocketsClient.h>
#include <base64.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <TCA9534.h>
#include "LovyanGFX_Driver.h"
#include "myMIC.h"
#include "ui.h"



LGFX tft;
TCA9534 ioex;
WebSocketsClient webSocket;


bool isOpenMic = false;  

/* Change to your screen resolution */
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;
static lv_color_t *buf1;
uint16_t touch_x, touch_y;

#define LCD_H_RES 800
#define LCD_V_RES 480





//wifi
const char *ssid = "YOUR-SSID";
const char *password = "YOUR-PASSWORD";


const char *websocket_server = "YOUR--COMPUTER-IP-ADDRESS"; 

const int websocket_port = 27000;
const char *websocket_path = "/";


String macAddress = "elecrow_meeting";

void setup() {
  Serial.begin(115200);
  init_screen();
  init_MIC();
  init_wifi();
  init_ws();
  Serial.println("success");

}

void loop() {
  webSocket.loop();
  if (isOpenMic) {
    record_send_audio();
  }
  lv_timer_handler();
  delay(1);
}


#ifdef __cplusplus
extern "C" {
#endif

  void sendTextToServer(const char *text);
  void send_signal_open();
  void send_signal_close();

#ifdef __cplusplus
}
#endif

void sendTextToServer(const char *text) {
  if (webSocket.isConnected()) {
    String record_data = createJsonString("record", text);
    webSocket.sendTXT(record_data);
    Serial.print("send: ");
    Serial.println(record_data);
    record_send_audio_wav();

  } else {
    Serial.println("error:WebSocket disconnect!");
  }
}


void send_signal_open() {
  String record_data = createJsonString("start_meeting", "");
  webSocket.sendTXT(record_data);
  Serial.print("start meeting");
  delay(5000);
  isOpenMic = true;
}

void send_signal_close(){
  isOpenMic=false;
  String jsonString = createJsonString("end_meeting", "");
  webSocket.sendTXT(jsonString);
}



void record_send_audio() {
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastTime >= (1000 / SAMPLE_RATE * SAMPLE_SIZE)) {
    lastTime = currentTime;

    size_t bytesRead;
    uint8_t i2sData[SAMPLE_SIZE * 2] = { 0 };  // 16位采样，每个采样2字节
    i2s_read(I2S_MIC_NUM, i2sData, SAMPLE_SIZE * 2, &bytesRead, portMAX_DELAY);

    if (bytesRead > 0) {
      String base64Data = base64::encode(i2sData, bytesRead);
      String jsonString = createJsonString("meeting_stream", base64Data);
      webSocket.sendTXT(jsonString);
      Serial.println("Send meeting audio data in real time");
    }
  }
}

void record_send_audio_wav() {
  Serial.println("Start recording and send it in real time...");

  unsigned long startTime = millis();
  unsigned long lastTime = 0;

  while (millis() - startTime < 5000) {  
    unsigned long currentTime = millis();

    if (currentTime - lastTime >= (1000 / SAMPLE_RATE * SAMPLE_SIZE)) {
      lastTime = currentTime;

      size_t bytesRead;
      uint8_t i2sData[SAMPLE_SIZE * 2] = { 0 };  // 16-bit samples

      i2s_read(I2S_MIC_NUM, i2sData, SAMPLE_SIZE * 2, &bytesRead, portMAX_DELAY);

      if (bytesRead > 0) {
        String base64Data = base64::encode(i2sData, bytesRead);
        String jsonString = createJsonString("recording_stream", base64Data);
        webSocket.sendTXT(jsonString);
        Serial.println("Real-time recorded audio data");
      }
    }
    delay(1);
  }

  String jsonString = createJsonString("recording_stream", "0x04");
  webSocket.sendTXT(jsonString);

  Serial.println("finish recording");
}


void init_screen() {
  Wire.begin(15, 16);
  delay(50);

  ioex.attach(Wire);
  ioex.setDeviceAddress(0x18);
  ioex.config(1, TCA9534::Config::OUT);
  ioex.config(2, TCA9534::Config::OUT);
  ioex.config(3, TCA9534::Config::OUT);
  ioex.config(4, TCA9534::Config::OUT);
  ioex.output(1, TCA9534::Level::H);

  // GT911 power on timing ->Select 0x5D
  pinMode(1, OUTPUT);
  digitalWrite(1, LOW);
  ioex.output(2, TCA9534::Level::L);
  delay(20);
  ioex.output(2, TCA9534::Level::H);
  delay(100);
  pinMode(1, INPUT);
  /*end*/

  tft.init();
  tft.initDMA();
  tft.startWrite();
  tft.fillScreen(TFT_BLACK);

  lv_init();
  size_t buffer_size = sizeof(lv_color_t) * LCD_H_RES * LCD_V_RES;
  buf = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
  buf1 = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);

  lv_disp_draw_buf_init(&draw_buf, buf, buf1, LCD_H_RES * LCD_V_RES);

  // Initialize display
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  // Change the following lines to your display resolution
  disp_drv.hor_res = LCD_H_RES;
  disp_drv.ver_res = LCD_V_RES;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // Initialize input device driver program
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  delay(100);

  tft.fillScreen(TFT_BLACK);
  // lv_demo_widgets();// Main UI interface
  ui_init();
}

void init_MIC() {
  i2s_driver_install(I2S_MIC_NUM, &i2s_config_mic, 0, NULL);
  i2s_set_pin(I2S_MIC_NUM, &pin_config_mic);
  i2s_zero_dma_buffer(I2S_MIC_NUM);
}

void init_wifi() {
  WiFi.begin(ssid, password);
  // Connect to the Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void init_ws() {
  webSocket.begin(websocket_server, websocket_port, websocket_path);
  webSocket.onEvent(webSocketEvent);
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket connected");
      break;
    case WStype_TEXT:
      {
        Serial.printf("Received text data: %s\n", payload);
        String rcv_word = (char *)payload; 

        if (rcv_word == "close_mic") {
          Serial.println("Turn off the microphone");

        } else if (rcv_word == "finish_tts") {
          delay(1000);
        }
        break;
      }

    case WStype_BIN:
      {

        break;
      }
  }
}


String createJsonString(const String &type, const String &data) {
  // Build a JSON object
  StaticJsonDocument<300> jsonDoc;
  // Create a nested JSON structure
  jsonDoc["event"] = type;
  jsonDoc["data"] = data;  // Use data as a sub-item under macAddress
  // Serialize the JSON object into a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  return jsonString;  // Return the serialized JSON string
}



//  Display refresh
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  if (tft.getStartCount() > 0) {
    tft.endWrite();
  }
  tft.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::rgb565_t *)&color_p->full);

  lv_disp_flush_ready(disp);  //	Tell lvgl that the refresh is complete
}

//  Read touch
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  data->state = LV_INDEV_STATE_REL;  // The state of data existence when releasing the finger
  bool touched = tft.getTouch(&touch_x, &touch_y);
  if (touched) {
    data->state = LV_INDEV_STATE_PR;

    //  Set coordinates
    data->point.x = touch_x;
    data->point.y = touch_y;
  }
}

