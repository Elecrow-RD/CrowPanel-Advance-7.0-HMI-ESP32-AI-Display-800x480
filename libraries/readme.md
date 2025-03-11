## Library File Explanation

**This is an explanation of the library files used for CrowPanel Advance HMI ESP32 AI Display.**

------

**Adafruit_BusIO**: Used when you need to communicate with sensors or devices connected via the I2C or SPI bus.

**Adafruit_GFX_Library**: A graphics library used for drawing graphics, text, lines, etc., on various displays. Use this when displaying graphical interfaces on OLED, LCD, or other screens.

**Adafruit_SSD1306**: Used when working with OLED displays based on the SSD1306 controller.

**arduino-audiokit-main**: Used when processing audio signals, such as playback, recording, and effects processing.

**arduinoFFT**: Used when performing frequency spectrum analysis on audio or other signals.

**Crowbits_DHT20**: Used for reading data from the DHT20 temperature and humidity sensor. Use this when measuring temperature and humidity.

**ESP32-audioI2S-3.0.12**: Used when playing audio via the ESP32's I2S interface.

(Use this when playing audio files or streaming audio on ESP32, such as making an audio player or smart speaker.)

**ESP32-AudioInI2S-master**: Used when recording audio via the ESP32's I2S interface.

(Use this when recording audio or processing microphone input, such as for audio signal analysis.)

**gt911-arduino-main**: Used when working with GT911 touchscreens.

**I2C_BM8563_RTC**: Used when working with the BM8563 real-time clock (RTC) module.

**I2S**: Used when processing audio via the I2S interface.

(Use this when directly controlling the I2S bus, such as implementing custom audio processing on ESP32 or communicating with other I2S devices.)

**LovyanGFX-master**: Used when drawing complex graphics or animations on a display.

**lvgl**: Used when creating graphical interfaces (such as buttons, sliders, and windows) on a display.

**PCA9557**: A library for driving the PCA9557 I2C GPIO expander. Used when expanding GPIO pins.

(If you need **high current drive capability** or **hardware reset functionality**, choose **PCA9557**. This is used in Lesson 6 with the nRF2401 module.)

**RadioLib**: A wireless communication library supporting various wireless modules (such as LoRa, Wi-Fi, and Bluetooth). Used when performing wireless communication.

**RF24-master**: A library for driving the nRF24L01 wireless module. Used when performing wireless communication with nRF24L01.

**SX126x-Arduino-master**: Used when performing LoRa communication with the SX126x module.

**TCA9534**: A library for driving the TCA9534 I2C GPIO expander. Used when expanding GPIO pins.

(If you need **low power consumption** and **a wider operating voltage range**, choose **TCA9534**.)

**UI**: A user interface-related library (project-specific, such as in Lesson 3, which uses the LVGL graphics library).

**lv_conf.h**: The configuration file for the lvgl graphics library, used to customize the behavior of the graphics library. Modify this file when configuring graphical functions in the lvgl library.