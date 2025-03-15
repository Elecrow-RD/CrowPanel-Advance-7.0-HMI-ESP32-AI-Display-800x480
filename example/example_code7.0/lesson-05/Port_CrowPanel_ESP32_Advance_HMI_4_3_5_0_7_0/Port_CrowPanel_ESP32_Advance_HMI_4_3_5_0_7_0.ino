#include "pins_config.h"
#include "LovyanGFX_Driver.h"
#include <Arduino.h>
#include <lvgl.h>
#include <Wire.h>
#include <SPI.h>
#include <stdbool.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <demos/lv_demos.h>
#include <Crowbits_DHT20.h> // Library files required for temperature and humidity sensors
#include "ui.h"

/* Expand IO */
#include <TCA9534.h>
TCA9534 ioex;

LGFX gfx;
Crowbits_DHT20 dht20;

/* Change to your screen resolution */
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;
static lv_color_t *buf1;

uint16_t touch_x, touch_y;

//  Display refresh
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  if (gfx.getStartCount() > 0) {
    gfx.endWrite();
  }
  gfx.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::rgb565_t *)&color_p->full);
  lv_disp_flush_ready(disp);  //	Tell lvgl that the refresh is complete
}

//  Read touch
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
  data->state = LV_INDEV_STATE_REL;// The state of data existence when releasing the finger
  bool touched = gfx.getTouch( &touch_x, &touch_y );
  if (touched)
  {
    data->state = LV_INDEV_STATE_PR;
    //  Set coordinates
    data->point.x = touch_x;
    data->point.y = touch_y;
  }
}
// Define the pins of the lamp
const int ledPin = 19;

void setup()
{
  Serial.begin(115200); 

  pinMode(19, OUTPUT);

  Wire.begin(15, 16);
  delay(50);
  dht20.begin();// Initialize temperature and humidity

  ioex.attach(Wire);
  ioex.setDeviceAddress(0x18);
  ioex.config(1, TCA9534::Config::OUT);// Backlight pin
  ioex.config(2, TCA9534::Config::OUT);// Touch pins
  /* Turn on backlight*/
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

  gfx.fillScreen(TFT_BLACK);
  ui_init();
  Serial.println( "Setup done" );
}

void loop()
{
  char DHT_buffer[10];
  int temp = (int)dht20.getTemperature();
  int humi = (int)dht20.getHumidity();

  snprintf(DHT_buffer, sizeof(DHT_buffer), "%d", temp);
  lv_label_set_text(ui_TempLabel1, DHT_buffer);
  // Lights up when the temperature exceeds the threshold
  if (temp > 30) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
  memset(DHT_buffer, 0, sizeof(DHT_buffer));
  
  snprintf(DHT_buffer, sizeof(DHT_buffer), "%d", humi);
  lv_label_set_text(ui_HumiLabel2, DHT_buffer);
  
  lv_timer_handler(); /* let the GUI do its work */
  delay(1);
}
