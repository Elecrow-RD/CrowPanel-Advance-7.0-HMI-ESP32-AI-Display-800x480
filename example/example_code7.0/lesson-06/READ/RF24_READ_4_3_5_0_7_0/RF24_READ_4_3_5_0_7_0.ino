#include <SPI.h>
#include <Wire.h>
#include <nRF24L01.h>
#include <RF24.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <driver/i2c.h>

class LGFX : public lgfx::LGFX_Device {
  public:

    lgfx::Bus_RGB _bus_instance;
    lgfx::Panel_RGB _panel_instance;
    //  lgfx::Light_PWM _light_instance;
    //  lgfx::Touch_GT911 _touch_instance;

    LGFX(void) {
      {
        auto cfg = _panel_instance.config();

        cfg.memory_width = 800;
        cfg.memory_height = 480;
        cfg.panel_width = 800;
        cfg.panel_height = 480;

        cfg.offset_x = 0;
        cfg.offset_y = 0;

        _panel_instance.config(cfg);
      }

      {
        auto cfg = _panel_instance.config_detail();

        cfg.use_psram = 1;

        _panel_instance.config_detail(cfg);
      }

      {
        auto cfg = _bus_instance.config();
        cfg.panel = &_panel_instance;
        cfg.pin_d0 = GPIO_NUM_21;    // B0
        cfg.pin_d1 = GPIO_NUM_47;    // B1
        cfg.pin_d2 = GPIO_NUM_48;   // B2
        cfg.pin_d3 = GPIO_NUM_45;    // B3
        cfg.pin_d4 = GPIO_NUM_38;    // B4
        cfg.pin_d5 = GPIO_NUM_9;    // G0
        cfg.pin_d6 = GPIO_NUM_10;    // G1
        cfg.pin_d7 = GPIO_NUM_11;    // G2
        cfg.pin_d8 = GPIO_NUM_12;   // G3
        cfg.pin_d9 = GPIO_NUM_13;   // G4
        cfg.pin_d10 = GPIO_NUM_14;   // G5
        cfg.pin_d11 = GPIO_NUM_7;  // R0
        cfg.pin_d12 = GPIO_NUM_17;  // R1
        cfg.pin_d13 = GPIO_NUM_18;  // R2
        cfg.pin_d14 = GPIO_NUM_3;  // R3
        cfg.pin_d15 = GPIO_NUM_46;  // R4

        cfg.pin_henable = GPIO_NUM_42;
        cfg.pin_vsync = GPIO_NUM_41;
        cfg.pin_hsync = GPIO_NUM_40;
        cfg.pin_pclk = GPIO_NUM_39;
        cfg.freq_write = 21000000;

        cfg.hsync_polarity = 0;
        cfg.hsync_front_porch = 8;
        cfg.hsync_pulse_width = 4;
        cfg.hsync_back_porch = 8;
        cfg.vsync_polarity = 0;
        cfg.vsync_front_porch = 8;
        cfg.vsync_pulse_width = 4;
        cfg.vsync_back_porch = 8;
        cfg.pclk_idle_high = 1;

        _bus_instance.config(cfg);
      }
      _panel_instance.setBus(&_bus_instance);

      //    {
      //      auto cfg = _light_instance.config();
      //      cfg.pin_bl = GPIO_NUM_2;
      //      _light_instance.config(cfg);
      //    }
      //    _panel_instance.light(&_light_instance);

      //    {
      //      auto cfg = _touch_instance.config();
      //      cfg.x_min = 0;
      //      cfg.x_max = 800;
      //      cfg.y_min = 0;
      //      cfg.y_max = 480;
      //      cfg.pin_int = GPIO_NUM_NC;
      //      cfg.bus_shared = false;
      //      cfg.offset_rotation = 0;
      //      // I2C接続
      //      cfg.i2c_port = I2C_NUM_0;
      //      cfg.pin_sda = GPIO_NUM_15;
      //      cfg.pin_scl = GPIO_NUM_16;
      //      cfg.pin_rst = -1;
      //      cfg.freq = 400000;
      //      cfg.i2c_addr = 0x5D;  // 0x5D , 0x14
      //      _touch_instance.config(cfg);
      //      _panel_instance.setTouch(&_touch_instance);
      //    }
      setPanel(&_panel_instance);
    }
};

LGFX gfx;

#include <PCA9557.h>
PCA9557 Out;

#define CE_PIN 20
#define CSN_PIN 19

// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

SPIClass* hspi = nullptr;

#define HSPI_MISO  4
#define HSPI_MOSI  6
#define HSPI_SCLK  5
#define HSPI_SS    19

/*
Function function: Display text on the screen
    lcd_w: Product horizontal axis resolution
    lcd_h： Product vertical axis resolution
    x： Screen displays the starting horizontal axis
    y： Screen displays the starting vertical axis
    text： The text content displayed on the screen
*/
void show_test(int lcd_w, int lcd_h, int x, int y, const char * text)
{
  gfx.fillScreen(TFT_BLACK);
  gfx.setTextSize(3);
  gfx.setTextColor(TFT_RED);
  gfx.setCursor(x, y);
  gfx.print(text); // 显示文本
}

const byte address[6] = "00001";
void setup() {
  Serial.begin(115200);

  Wire.begin(15, 16);// 4.3 size, 5.0 size, and 7.0 size use IIC controlled extended IC chips
  delay(50);
  //  Backlight pin
  Out.reset();// The backlight is controlled by the aforementioned extended IC chip, so the writing is different here. 
  Out.setMode(IO_OUTPUT); //  The purpose of this code is to turn on the screen backlight, allowing the screen to display
  Out.setState(IO1, IO_HIGH);

  // Init Display
  gfx.init();
  gfx.initDMA();
  gfx.startWrite();
  gfx.fillScreen(TFT_BLACK);

  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }

  hspi = new SPIClass(HSPI); // by default VSPI is used
  // to use the custom defined pins, uncomment the following
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);

  if (!radio.begin(hspi)) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  else
  {
    Serial.println(F("radio hardware is OK!!"));
  }
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);  //RF24_250KBPS  RF24_1MBPS  RF24_2MBPS
  radio.setChannel(50);
  radio.startListening();
}

int i=0;
void loop() {                                                                 
  //  Serial.println(F("READ !!"));
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));//  Read the content of the text sent over
    Serial.println(text);
    String str = text;
    str += String(i);
    show_test(800, 480, 300, 230, str.c_str());
    i++;
  }
}
