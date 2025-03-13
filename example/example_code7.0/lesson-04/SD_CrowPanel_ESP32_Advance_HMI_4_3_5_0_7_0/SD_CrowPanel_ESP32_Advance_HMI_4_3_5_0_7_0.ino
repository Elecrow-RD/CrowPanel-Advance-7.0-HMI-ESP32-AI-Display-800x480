#include "pins_config.h"
#include "LovyanGFX_Driver.h"
#include <Wire.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

/* Expand IO */
#include <TCA9534.h>
TCA9534 ioex;

#define SD_MOSI 6
#define SD_MISO 4
#define SD_SCK 5
#define SD_CS 0 //The chip selector pin is not connected to IO

// The image name you stored on the SD card
#define IMAGE_1 "/1.bmp"
#define IMAGE_2 "/2.bmp"
#define IMAGE_3 "/3.bmp"
#define IMAGE_4 "/4.bmp"
#define IMAGE_5 "/5.bmp"

SPIClass SD_SPI = SPIClass(HSPI);
LGFX gfx;

// Display text on the screen
void show_test(int lcd_w, int lcd_h, int x, int y, const char * text) {
  gfx.fillScreen(TFT_BLACK);
  gfx.setTextSize(3);
  gfx.setTextColor(TFT_RED);
  gfx.setCursor(x, y);
  gfx.print(text); 
}

void setup()
{
  Serial.begin(115200); // set baud rate

 // Init Display
  gfx.init();
  gfx.initDMA();
  gfx.startWrite();
  gfx.fillScreen(TFT_BLACK);
  delay(500);

  Wire.begin(15, 16);
  delay(50);
  ioex.attach(Wire);
  ioex.setDeviceAddress(0x18);
  ioex.config(1, TCA9534::Config::OUT);
  /* Turn on backlight */
  ioex.output(1, TCA9534::Level::H);

  if (SD_init() == 0)
  {
    Serial.println("TF_Card initialization succeeded");
    show_test(LCD_H_RES, LCD_V_RES, 350, 230, "SD_Card OK");
    delay(3000);
  } else {
    Serial.println("TF card initialization failed");
    show_test(LCD_H_RES, LCD_V_RES, 350, 230, "SD_Card failed");
    delay(3000);
  }
  gfx.setRotation(2);
  gfx.fillScreen(TFT_BLACK);
  Serial.println( "----- Setup done -----" );
}

void loop()
{
 // Cycle printing each image
  Serial.println("Refreshing image...1");
  displayImage(SD, IMAGE_1, 800, 480);
  delay(5000);

  Serial.println("Refreshing image...2");
  displayImage(SD, IMAGE_2, 800, 480);
  delay(5000);

  Serial.println("Refreshing image...3");
  displayImage(SD, IMAGE_3, 800, 480);
  delay(5000);

  Serial.println("Refreshing image...4");
  displayImage(SD, IMAGE_4, 800, 480);
  delay(5000);

  Serial.println("Refreshing image...5");
  displayImage(SD, IMAGE_5, 800, 480);
  delay(5000);
}

// SD card initialization
int SD_init()
{
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  SD_SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
  if (!SD.begin(SD_CS, SD_SPI, 80000000))
  {
    Serial.println(F("ERROR: File system mount failed!"));
    SD_SPI.end();
    return 1;
  }
  else
  {
    Serial.println("Card Mount Successed");
    Serial.printf("SD Size: %lluMB \n", SD.cardSize() / (1024 * 1024));
    // hspi->end();
  }
  listDir(SD, "/", 2);
  Serial.println("**** TF Card init finished ****.");
  return 0;
}

/*
Function: List files and subdirectories in the specified directory.
Parameters:
  - fs: The file system object used to operate on the file system.
  - dirname: A pointer to the name of the directory whose contents are to be listed.
  - levels: Specifies the depth of recursive listing of subdirectories.
*/
void listDir(fs::FS & fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname); 
  File root = fs.open(dirname); // Open the specified directory.
  // If the directory cannot be opened.
  if (!root) { 
    Serial.println("Failed to open directory"); 
    return; 
  }
  // If the opened item is not a directory.
  if (!root.isDirectory()) { 
    Serial.println("Not a directory"); 
    return; 
  }
  File file = root.openNextFile(); // Open the next file in the directory.
  // While there are still files.
  while (file) { 
    // If the current file is a directory.
    if (file.isDirectory()) { 
      Serial.print("  DIR : "); 
      Serial.println(file.name()); // Print the directory name.
      // If there are still levels of depth for recursion.
      if (levels) { 
        listDir(fs, file.name(), levels - 1); // Recursively call the function to list the contents of the subdirectory, reducing the depth by 1.
      }
    } 
    // If the current file is not a directory, i.e., a regular file.
    else { 
      Serial.print("  FILE: "); 
      Serial.print(file.name()); // Print the file name.
      Serial.print("  SIZE: "); 
      Serial.println(file.size()); // Print the file size.
    }
    file = root.openNextFile(); // Continue to open the next file in the directory.
  }
}

/*
  Function:
    - Display image from file
  Parameters:
    - fs: The file system object
    - filename: The name of the BMP image file
    - x: The x - coordinate on the screen to start printing the image
    - y: The y - coordinate on the screen to start printing the image
  Returns:
    - 0 if the image is printed successfully, 0 if the file cannot be opened
*/
int displayImage(fs::FS &fs, String filename, int x, int y)
{
    File f = fs.open(filename, "r"); // Open the file for reading
    if (!f)
    {
        Serial.println("Failed to open file for reading");
        f.close();
        return 0; // File open failed, return 0
    }
    f.seek(54); // Skip the 54 - byte BMP file header
    int X = x; // representing the width of the image
    int Y = y; // representing the height of the image
    uint8_t RGB[3 * X]; // Storage space for RGB data of each row, 320 pixels, 3 bytes per pixel (RGB)

    for (int row = 0; row < Y; row++) // Iterate through each row of the image
    {
        f.seek(54 + 3 * X * row); // Jump to the position of the current row
        f.read(RGB, 3 * X); // Read the RGB data of the current row
        gfx.pushImage(0, row, X, 1, (lgfx::rgb888_t *)RGB); // Display the data of the current row
    }
    f.close(); // Close the file
    return 0; 
}


