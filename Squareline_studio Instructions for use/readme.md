# Use SquareLine Studio and LVGL libraries to create a UI interface to light the lights

## **1.** Introduce LVGL

------

LVGL (LittlevGL) is an open-source, lightweight, high-performance embedded graphics library designed specifically for devices with limited resources. It supports multi platform porting, provides rich controls, animations, touch support, and highly customizable styles, suitable for fields such as smart homes, industrial equipment, medical instruments, etc. LVGL is centered around modular design and can run on bare metal or operating systems, accelerating GUI development through powerful community support and tools such as SquareLine Studio.

![1](https://www.elecrow.com/pub/wiki/assets/images/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/1.png)

SquareLine Studio is a next-generation user interface (UI) solution for individuals and professionals, allowing users to quickly and easily design and develop aesthetically pleasing UI for embedded devices. This software provides integrated design, prototyping, and development capabilities, supporting the export of platform independent C or MicroPython code for LVGL (Lightweight Graphics Library), which can be compiled and run on any vendor's device.

![2](https://www.elecrow.com/pub/wiki/assets/images/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/2.png)

## 2.Install SquareLine Studio

------

### **2.1** Installation Guide

Enter the https://squareline.io/ to download the SquareLine installation file.

![3](https://www.elecrow.com/pub/wiki/assets/images/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/3.png)

Download the version 1.4.0

![4](https://www.elecrow.com/pub/wiki/assets/images/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/4.png)

Double-click the setup.exe file.

![5](https://www.elecrow.com/pub/wiki/assets/images/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/5.png)

Click install.

![6](https://www.elecrow.com/pub/wiki/assets/images/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/6.png)

Wait for installation.

![7](https://www.elecrow.com/pub/wiki/assets/images/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/7.png)

Installation finish.

![8](https://www.elecrow.com/pub/wiki/assets/images/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/8.png)

There is a 30-day trial period for the first time you use it. Please follow the prompts to register an account. You will continue to use it when you log in to your account next time.

![9](https://www.elecrow.com/pub/wiki/assets/images/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/9.png)

### **2.2** Software Function Introduction

Open the software

The historical project page: open the project built earlier.

![10](https://www.elecrow.com/pub/wiki/assets/images/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/10.png)

Create a project page: choose different platforms according to different hardware of the project.

![11](https://www.elecrow.com/pub/wiki/assets/images/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/11.png)

###### When select the Arduino framwork, there's only one option "Arduino with TFT_eSPI". By choosing this, the squareline studio will generate a template code suitable for the TFT_eSPI library. However, squareline studio not only supports the TFT_eSPI library, it supports a variety of libraries to suit different hardware and application needs. For example, Adafruit_GFX library, LovyanGFX etc.[¶](https://www.elecrow.com/pub/wiki/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights.html#when-select-the-arduino-framwork-theres-only-one-option-arduino-with-tft_espi-by-choosing-this-the-squareline-studio-will-generate-a-template-code-suitable-for-the-tft_espi-library-however-squareline-studio-not-only-supports-the-tft_espi-library-it-supports-a-variety-of-libraries-to-suit-different-hardware-and-application-needs-for-example-adafruit_gfx-library-lovyangfx-etc)

###### After using SLS to generate UI code, we then use different graphics libraries according to different hardware and modify the corresponding code to display the content you design.[¶](https://www.elecrow.com/pub/wiki/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights.html#after-using-sls-to-generate-ui-code-we-then-use-different-graphics-libraries-according-to-different-hardware-and-modify-the-corresponding-code-to-display-the-content-you-design)

Example page. This page has several official examples for reference.

![12](https://www.elecrow.com/pub/wiki/assets/images/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/12.png)

The project settings bar is used to make basic settings for the project, including property settings such as project name, screen size, display angle, etc.

![13](https://www.elecrow.com/pub/wiki/assets/images/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/13.png)

###### Note： Please select the corresponding resolution and color depth according to the screen specifications.[¶](https://www.elecrow.com/pub/wiki/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights.html#note-please-select-the-corresponding-resolution-and-color-depth-according-to-the-screen-specifications)

![14](https://www.elecrow.com/pub/wiki/assets/images/3.5_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/14.png)

##### ①Toolbar, including File, Export, and Help. Basic file operation bar, create or open files, export UI files and other operations. Click help and there are related introductory documents.

##### ②Screen bar, the project screen will be listed here.

##### ③Widget area, all widgets are here and can be selected and used according to project needs.

##### ④Hierarchy area, it will show every widget used in each screen.

##### ⑤This area shows the actual display effect, you can adjust the widgets or screen here.

##### ⑥Material column, the added materials are displayed here.

##### ⑦Setting bar, where you can make basic settings for each part, including the basic attributes and trigger operations of the part.

##### ⑧Theme bar, different themes can be set.

## 3 Use SquareLine Studio to create UI control interfaces

------

Firstly, open the SquareLine Studio software and create a case study

![15](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/15.png)

Choose the correct resolution based on the different sizes you are using

![16](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/16.png)

Here, I take a 5.0-inch screen as an example. After determining the resolution, I fill it in

![17](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/17.png)

After selecting the parameters, click Create

![18](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/18.png)

Open the photo of the desk lamp we provided and add it in. (Of course, you can also choose the image you want to use)

![19](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/19.png)

![20](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/20.png)

The lamp_28 image size is available for both 2.4-inch and 2.8-inch screens.

Table_1amp image sizes are available for 3.5-inch, 4.3-inch, 5.0-inch, and 7-inch screens.

![21](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/21.png)

After adding it, drag and drop the image in.

![22](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/22.png)

The task we need to complete is to turn on and off the lights by clicking on the buttons on the graphical interface. So we need to design two buttons

From the left sidebar, select Button and drag it into the interface.

![23](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/23.png)

You can adjust the border of the button with the mouse, which can adjust the size of the button, and you can also drag and drop the button to adjust its position.

Then, by selecting Background, choose the background color of the Button.

![24](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/24.png)

![25](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/25.png)

We have made the button patterns, and now we need to add labels to the button patterns in order to distinguish their functions.

Drag and drop from the left sidebar, select Label, and drag into the interface.

![26](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/26.png)

Modify the text content of the label

![27](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/27.png)

And modify the font size and text color of the text

![28](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/28.png)

The Button and Label have been designed. Click on the Hierarchy in the right-hand column and drag the Label onto the Button line to merge them into one.

![29](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/29.png)

At this point, if you drag the buttons again, you will find that they are dragged together

Next, we will copy a completed button, right-click, and paste it in.

![30](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/30.png)

![31](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/31.png)

![32](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/32.png)

Click the second button ON to change the text content to Off. Used to achieve the effect of turning off lights.

![33](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/33.png)

## **4** Add functions to the buttons to enable them to turn on and off lights

------

Select 'event' to add in the right sidebar

![34](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/34.png)

Select 'released' as the triggering condition and 'Call function' as the action.

![35](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/35.png)

After selecting, click ADD.

And add a function name to the CALL Function. (Customization is sufficient)

![36](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/36.png)

Similarly, add an event to the Off button. The operation process is the same as above.

![37](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/37.png)

After adding, run

![38](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/38.png)

![39](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/39.png)

## **5** UI interface design completed, exporting UI files for easy use in subsequent code

------

Click on Project Setting

![40](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/40.png)

Set export path

![41](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/41.png)

![42](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/42.png)

Complete the setup and export file

![43](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/43.png)

Copy and paste the exported code into the code folder we need to open

![44](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/44.png)

![45](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/45.png)

Then double-click BigInch_LVGL.ino and open it using Arduino IDE

## **6 Connect the light bulb and add code to control the light bulb to turn on and off**

------

Connect the light bulb at the UART1 interface

![46](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/46.png)

Observing the schematic diagram of this size, it is known that the pin for UART1 to control the light bulb to turn on and off is 19

![47](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/47.png)

![48](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/48.png)

A large-sized screen requires setting 19 pins as the output mode.

(Due to the fact that pin 19 in the circuit of large-sized products controls both the wireless module and the UART1 interface, it is necessary to switch the mode to 0 1)

![49](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/49.png)

![49-1](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/49-1.png)

And add the function of turning on and off the light bulb in the ui_ event. c file.

(When the On button is clicked, the light is on; when the Off button is clicked, the light is off)

![50](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/50.png)

## **7 Configure the code running environment and upload the code**

------

(!! Before uploading the code, please use different library files according to the size you are using. You can review the content of Lesson 2)

(For large-sized screens of 4.3 inches, 5.0 inches, and 7.0 inches, be sure to switch the mode to 0 1 state before uploading the code, because the pins of UART1 and W-M mode are both used in 0 1 mode, so that you can use the pins of UART1)

![51](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/51.png)

![52](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/52.png)

So you can see the UI interface you just designed

![53](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/53.png)

You can click the On and Off buttons to control the on/off of the lights

![54](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/54.png)

![55](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/55.png)

## 8 Code presentation

------

### 7.0 inches, 5.0 inches, 4.3 inches

```
#include "pins_config.h"
#include "LovyanGFX_Driver.h"

#include <Arduino.h>
#include <lvgl.h>
#include <Wire.h>
#include <SPI.h>

#include <stdbool.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>



/* Expand IO */
#include <TCA9534.h>
TCA9534 ioex;

LGFX gfx;

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

  lv_disp_flush_ready(disp);  //    Tell lvgl that the refresh is complete
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

void setup()
{
  Serial.begin(115200); 

  pinMode(19, OUTPUT);

  Wire.begin(15, 16);
  delay(50);

  ioex.attach(Wire);
  ioex.setDeviceAddress(0x18);
  ioex.config(1, TCA9534::Config::OUT);
  ioex.config(2, TCA9534::Config::OUT);
  ioex.config(3, TCA9534::Config::OUT);
  ioex.config(4, TCA9534::Config::OUT);

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
  // lv_demo_widgets();// Main UI interface
  ui_init();

  Serial.println( "Setup done" );
}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  delay(1);
}
```

**If your code compiles incorrectly, you can check if the ESP32 version number is correct. The ESP32 version number we need for this lesson is 3.0.2.**

![new-1](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/new-1.png)

**Secondly, please pay attention to replacing the corresponding size library file.**

**Select the appropriate library file based on the product screen size**

Path reference：C:\ESP32_Code\CrowPanel_Advanced_HMI\Arduino_lib Series Library

![new-2](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/new-2.png)

**I will use the Advance 7.0-inch product as an example for operation**

Copy the Libraries Advanced 7.0 folder

Open Arduino IDE runtime library file path

Reference path: C:\Users\14175\Documents\Arduino

![new-3](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/new-3.png)

Delete the existing libraries folder

![new-4](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/new-4.png)

Paste the copied library Advanced 7.0 folder into this path

![new-5](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/new-5.png)

Change the folder name to the original libraries

![new-6](https://www.elecrow.com/pub/wiki/assets/images/5.0_3_Use_LVGL_library_to_create_UI_interface_and_light_up_lights/new-6.png)

Library update completed, restart Arduino IDE.

**When using other sizes, changing the library file is the same operation**