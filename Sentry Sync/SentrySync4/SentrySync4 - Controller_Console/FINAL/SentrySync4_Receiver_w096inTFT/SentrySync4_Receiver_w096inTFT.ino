#include <ArduinoOTA.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h> 
#include <SD.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <esp_now.h>
#include "JFresh_AI_128x64.h" 

//   Using ESP32 C3 Dev board  HAVE TO USE esp board library v2.0.14!!
//----------------------------------------------------------------------------------
#define TFT_CS   5  // Chip select pin
#define TFT_RST  4   // Reset pin
#define TFT_DC   2   // Data/Command pin
#define TFT_MOSI 8  // MOSI (SPI data)
#define TFT_SCLK 9  // SCK (SPI clock)
/*

//   Using ESP32 WROOM Dev board  HAVE TO USE esp board library v3.2.0!!
//----------------------------------------------------------------------------------
#define TFT_CS   15  // Chip select pin
#define TFT_RST  4   // Reset pin
#define TFT_DC   2   // Data/Command pin
#define TFT_MOSI 21  // MOSI (SPI data)
#define TFT_SCLK 22  // SCK (SPI clock)
*/
#define UART_BAUD_RATE 115200

TFT_eSPI tft = TFT_eSPI();

const char* ssid = "NETWORK NAME";
const char* password = "PASSWORD";

bool uartMode = false; // Variable to track the uart mode
bool sdStatus = false; // Variable to track if sd card is present
bool tidStatus = false; // Variable to track if title id was sent via uart
// Variables to store sensor data
float gpuTemp = 0.0;
float cpuTemp = 0.0;
float memTemp = 0.0;
float caseTemp = 0.0;
String gameTitle;
String titleID;
String developer;
String ipAddress;
String servipAddress;
String lastID = "";

unsigned long previousMillis = 0;
const long interval = 2000; // Interval to update OLED display (milliseconds)

//Colors--------MUST be in BGR format-----------------------------------------
uint16_t XBGreen = tft.color565(0, 160, 48);  // Xbox green
uint16_t xYellow = tft.color565(0, 255, 255);  
uint16_t CoolBlue = tft.color565(124, 89, 24);  // SD card blue
uint16_t xRed = tft.color565(0, 0, 255);
uint16_t xPurple = tft.color565(255, 0, 100);
uint16_t xBlue = tft.color565(255, 0, 0);
uint16_t xOrange = tft.color565(0, 100, 255);
uint16_t xPink = tft.color565(255, 0, 255);
uint16_t xGray = tft.color565(100, 100, 100);

// 'sentrysplash', 128x64px
const unsigned char epd_bitmap_sentrysplash [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0xe0, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x80, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xc0, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xe0, 0x00, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xc0, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xc0, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x80, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x1f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0xe0, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfc, 0x01, 0xf0, 0x07, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0x03, 0xf8, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0x07, 0xfc, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0x0f, 0xfe, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0x1f, 0xff, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x3f, 0xff, 0x80, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x7f, 0xff, 0xc0, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0xff, 0xff, 0xe0, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0xff, 0xff, 0xe0, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x81, 0xff, 0xff, 0xf0, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x83, 0xff, 0xff, 0xf8, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x87, 0xff, 0xff, 0xfc, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0xff, 0xff, 0xfc, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0f, 0xff, 0xff, 0xfe, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x1f, 0xff, 0xff, 0xff, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1f, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'question', 128x64px
const unsigned char epd_bitmap_question [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xff, 0xff, 0xfd, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x10, 0x88, 0xc5, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x10, 0x88, 0xc5, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x10, 0x88, 0xc5, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x10, 0x88, 0xc5, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x10, 0x88, 0xc5, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x10, 0x88, 0xc5, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x10, 0x88, 0xc5, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x10, 0x88, 0xc5, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x10, 0x88, 0xc5, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xff, 0xff, 0xfd, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x07, 0xfc, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x07, 0xfc, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x1f, 0xff, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x1e, 0x0f, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1c, 0x0f, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1c, 0x0f, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x0f, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x0f, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x1f, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x3e, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x7c, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0xf8, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0xf0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0xe0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0xe0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0xe0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0xe0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//----------------------------------------------------------------------------------
//WiFi connection
boolean ConectWiFi(void){
  boolean state = true;
  byte c = 0;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    if (c > 20){
      state = false;
      break;}
    c++;}
  return state;}

// Structure to receive data
// Must match the sender structure
typedef struct struct_message {
  float gTemp;
  float cTemp;
  float eTemp;
  float mTemp;
  bool sd;
  bool uart;
  bool tidSent;
  char gameTitle[32];
  char developer[32];
  char tTitleID[32];
  char xboxipAdd[32];
} struct_message;

// struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  gpuTemp = myData.gTemp;
  cpuTemp = myData.cTemp;
  memTemp = myData.eTemp;
  caseTemp = myData.mTemp;
  sdStatus = myData.sd;
  uartMode = myData.uart;
  tidStatus = myData.tidSent;
  gameTitle = myData.gameTitle;
  developer = myData.developer;
  titleID = myData.tTitleID;
  ipAddress = myData.xboxipAdd;

  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("GPU: ");
  Serial.println(myData.gTemp);
  Serial.print("CPU: ");
  Serial.println(myData.cTemp);
  Serial.print("EDRAM: ");
  Serial.println(myData.eTemp);
  Serial.print("MOBO: ");
  Serial.println(myData.mTemp);
  Serial.print("SD Status: ");
  Serial.println(myData.sd);
  Serial.print("UART Mode: ");
  Serial.println(myData.uart);
  Serial.print("TiD Sent: ");
  Serial.println(myData.tidSent);
  Serial.print("Game Title: ");
  Serial.println(myData.gameTitle);
  Serial.print("Developer: ");
  Serial.println(myData.developer);
  Serial.print("Title ID: ");
  Serial.println(myData.tTitleID);
  Serial.print("IP Address: ");
  Serial.println(myData.xboxipAdd);
  Serial.println();
    
  if (myData.tidSent) { 
    tidStatus = true;         
  }
  if (!tidStatus){
    lastID = "";
  }
}

//----------------------------------------------------------------------------------
void setup() {
  Serial.begin(UART_BAUD_RATE);
  tft.init(ST7735_GREENTAB160x80); // Initialize TFT_eSPI
  tft.setRotation(ST7735_MADCTL_BGR);
  tft.setRotation(1); // Set display rotation (adjust as needed)
  tft.fillScreen(TFT_BLACK); // Clear the screen with black
  tft.setTextSize(1); // Set default text size
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // Set default text color to white

  WiFi.mode(WIFI_STA);
  delay(300);
  
  //Connect to Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  if(ConectWiFi()==true){
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("WiFi channel: ");
    Serial.println(WiFi.channel());}
  else{
    Serial.println("");
    Serial.println("WiFi not connected.");}
    
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    
  }
  else {
    Serial.println("initialized ESP-NOW!!");
  }
  
  // Once ESPNow is successfully Init, register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  // Display splash screen
  tft.drawBitmap(15, 3, epd_bitmap_sentrysplash, 128, 64, XBGreen);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 62);
  tft.println("  SentrySync");

  delay(5000); // Display splash screen for 5 seconds

  
  // Clear the display
  tft.fillScreen(TFT_BLACK);
 
  // Initialize SD card
  if (!sdStatus) {
    Serial.println("SD Mount Failed");
    tft.drawBitmap(15, 8, epd_bitmap_question, 128, 64, CoolBlue); // SD Card Image
    tft.setTextSize(1); // Set text size
    tft.setTextColor(xYellow); // Set text color
    tft.setCursor(20, 5); // Set cursor position
    tft.println("  SD Card Not Found!");
    delay(5000); // Wait for 5 seconds
    drawBackground();   
  }
  
  
  ArduinoOTA.begin();
  Serial.println("OTA started");

  if (ipAddress != "0.0.0.0") {
    // Display IP address on OLED screen      
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(0, 23);
    tft.println("Xbox IP Address:");
    tft.println(ipAddress);
    delay(5000); // Display for 5 seconds
    drawBackground(); 
  }
  else{
    delay(5000);
    drawBackground();
  }
}

//----------------------------------------------------------------------------------
void drawBackground() {
  for (int y = 0; y < 80; y++) {
    for (int x = 0; x < 160; x++) {
      // Read the pixel color from the image data
      uint16_t color = pgm_read_word(&JFresh_AI_128x64[y * 160 + x]);
      
      // Swap red and blue channels
      uint8_t red = (color >> 11) & 0x1F;  // Extract red (5 bits)
      uint8_t green = (color >> 5) & 0x3F; // Extract green (6 bits)
      uint8_t blue = color & 0x1F;         // Extract blue (5 bits)
      color = (blue << 11) | (green << 5) | red; // Reconstruct color with swapped channels
      
      // Draw the pixel
      tft.drawPixel(x, y, color);
    }
  }
}

//----------------------------------------------------------------------------------
void loop() {
  ArduinoOTA.handle();
  delay(100);//allow the cpu to switch to other tasks
  
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    updateOLED();
  }  
}

//----------------------------------------------------------------------------------
void updateOLED() {
  if (tidStatus && titleID != lastID) {
    displayInfoFromSD(titleID);
    lastID = titleID; // Update the last displayed title ID
    tidStatus = false;
  }
  
  static float lastGpuTemp = -1; // Store the last displayed values
  static float lastCpuTemp = -1;
  static float lastMemTemp = -1;
  static float lastCaseTemp = -1;

  // Only update the display if the values have changed
  if (gpuTemp != lastGpuTemp || cpuTemp != lastCpuTemp || memTemp != lastMemTemp || caseTemp != lastCaseTemp) {
    // Clear only the area where the temperatures are displayed
    tft.setTextSize(2);
    tft.setTextColor(TFT_BLACK, TFT_BLACK); // Set text color to black (to overwrite old text)
    tft.setCursor(0, 10);
    tft.print("GPU: ");
    tft.print(lastGpuTemp, 1);
    tft.print("C");
    tft.println();
    tft.print("CPU: ");
    tft.print(lastCpuTemp, 1);
    tft.print("C");
    tft.println();
    tft.print("EDR: ");
    tft.print(lastMemTemp, 1);
    tft.print("C");
    tft.println();
    tft.print(" MB: ");
    tft.print(lastCaseTemp, 1);
    tft.print("C");
    tft.println();

    // Update the last displayed values
    lastGpuTemp = gpuTemp;
    lastCpuTemp = cpuTemp;
    lastMemTemp = memTemp;
    lastCaseTemp = caseTemp;

    // Draw the new temperature values
    tft.setTextColor(xRed); // Set text color to white
    tft.setCursor(0, 10);
    tft.print("GPU: ");
    tft.print(gpuTemp, 1);
    tft.print("C");
    tft.println();
    tft.print("CPU: ");
    tft.print(cpuTemp, 1);
    tft.print("C");
    tft.println();
    tft.print("EDR: ");
    tft.print(memTemp, 1);
    tft.print("C");
    tft.println();
    tft.print(" MB: ");
    tft.print(caseTemp, 1);
    tft.print("C");
    tft.println();
  } 
}

//----------------------------------------------------------------------------------
void displayInfoFromSD(String targetTitleID) {

  // Display information on OLED screen
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(5, 5);
  tft.println(gameTitle);
  tft.println();
  tft.print("Title ID: ");
  targetTitleID.toUpperCase(); // Converts title id to uppercase
  tft.println(targetTitleID); // Displays title id in uppercase
  tft.println();
  tft.println("Developer:");
  tft.println(developer);    
  delay(8000); // Display for 10 seconds
  drawBackground();
}
