#include "DHT.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char* ssid = "DraadlozeWifiKabel";
const char* password = "GeefWifi!123";

bool WiFiConnected = false;
int methodTimeCounter = 0;

const long utcOffsetInSeconds = 7200;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

int countdown = 0;
const char* weather_main = "Unknown";
const char* weather_main_desc = "Unknown";
float temp = 0;
float humid = 0;
float windspeed = 0;

#define DHTPIN 12     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors
DHT dht(DHTPIN, DHTTYPE);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     LED_BUILTIN // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

#define LOGO_HEIGHT_WEATHER   32
#define LOGO_WIDTH_WEATHER    32

const unsigned char wifiLogo2_bmp[] PROGMEM = {
  B00000000,B00000000,
  B00000000,B00000000,
  B00001111,B11110000,
  B00111000,B00011000,
  B01110000,B00001110,
  B11000011,B11000011,
  B10001111,B11110001,
  B00011000,B00011000,
  B00110000,B00001100,
  B00100011,B11100100,
  B00000110,B01100000,
  B00001000,B00010000,
  B00000001,B10000000,
  B00000001,B10000000,
  B00000000,B00000000,
  B00000000,B00000000
};


static const unsigned char PROGMEM wifiLogo_bmp[] =
{
  B00000011, B11000000,
  B00001111, B11110000,
  B00111110, B01111100,
  B01111000, B00011110,
  B11100001, B10000111,
  B01000111, B11100010,
  B00011110, B01111000,
  B00111000, B00011100,
  B00010001, B10001000,
  B00000011, B11000000,
  B00000110, B01100000,
  B00000000, B00000000,
  B00000001, B10000000,
  B00000011, B11000000,
  B00000001, B10000000,
  B00000000, B00000000
  
};

static const unsigned char PROGMEM tempLogo_bmp[] =
{
  B00000111,B10000000,
  B00001100,B11000000,
  B00001000,B01000000,
  B00001000,B01000000,
  B00001000,B01000000,
  B00001000,B01000000,
  B00001011,B01000000,
  B00001011,B01000000,
  B00001011,B01000000,
  B00010111,B00100000,
  B00010111,B10100000,
  B00010111,B10100000,
  B00010111,B10100000,
  B00010011,B00100000,
  B00001100,B11000000,
  B00000111,B10000000
};

static const unsigned char PROGMEM humidLogo_bmp[] =
{
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000001, B10000000,
  B00000011, B11000000,
  B00000011, B11000000,
  B00000111, B11100000,
  B00000111, B00100000,
  B00001111, B10010000,
  B00001111, B10010000,
  B00001111, B10010000,
  B00000111, B00100000,
  B00000011, B11000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000
};

static const unsigned char windLogoSmall_bmp[] PROGMEM = {
  B00000000,B01111110,
  B00000000,B01100011,
  B00000000,B11000011,
  B00000000,B11000001,
  B00000000,B00000011,
  B00000000,B00001111,
  B01111111,B11111110,
  B01111111,B11111000,
  B00000000,B00000000,
  B11111111,B11100000,
  B00000000,B00000000,
  B11111111,B11100000,
  B00000000,B00110000,
  B00000011,B00011000,
  B00000011,B00110000,
  B00000001,B11100000
};

static const unsigned char PROGMEM sunLogo_bmp[] = {
  B00000000,B00000001,B10000000,B00000000,
  B00000000,B00000001,B10000000,B00000000,
  B00000000,B00000001,B10000000,B00000000,
  B00000000,B00000001,B10000000,B00000000,
  B00000100,B00000001,B10000000,B00100000,
  B00001110,B00000001,B10000000,B01110000,
  B00000111,B00000000,B00000000,B11100000,
  B00000011,B10000000,B00000001,B11000000,
  B00000001,B10000111,B11100001,B10000000,
  B00000000,B00011111,B11111000,B00000000,
  B00000000,B00111110,B01111100,B00000000,
  B00000000,B01111000,B00011110,B00000000,
  B00000000,B01110000,B00001110,B00000000,
  B00000000,B11100000,B00000111,B00000000,
  B00000000,B11100000,B00000111,B00000000,
  B11111100,B11000000,B00000011,B00111111,
  B11111100,B11000000,B00000011,B00111111,
  B00000000,B11100000,B00000111,B00000000,
  B00000000,B11100000,B00000111,B00000000,
  B00000000,B01110000,B00001110,B00000000,
  B00000000,B01111000,B00011110,B00000000,
  B00000000,B00111110,B01111100,B00000000,
  B00000000,B00011111,B11111000,B00000000,
  B00000001,B10000111,B11100001,B10000000,
  B00000011,B10000000,B00000001,B11000000,
  B00000111,B00000000,B00000000,B11100000,
  B00001110,B00000001,B10000000,B01110000,
  B00000100,B00000001,B10000000,B00100000,
  B00000000,B00000001,B10000000,B00000000,
  B00000000,B00000001,B10000000,B00000000,
  B00000000,B00000001,B10000000,B00000000,
  B00000000,B00000001,B10000000,B00000000 
};

static const unsigned char moonLogo_bmp[] PROGMEM = {
  B00000000,B00111000,B00000000,B00000000,
  B00000000,B11111100,B00000000,B00000000,
  B00000011,B11111000,B00000000,B00000000,
  B00000111,B11110000,B00000000,B00000000,
  B00001111,B11110000,B00000000,B00000000,
  B00011110,B11110000,B00000000,B00000000,
  B00111100,B11100000,B00000000,B00000000,
  B00111000,B11100000,B00000000,B00000000,
  B01111000,B11100000,B00000000,B00000000,
  B01110000,B11100000,B00000000,B00000000,
  B01110000,B11100000,B00000000,B00000000,
  B11100000,B11100000,B00000000,B00000000,
  B11100000,B11100000,B00000000,B00000000,
  B11100000,B01110000,B00000000,B00000000,
  B11100000,B01110000,B00000000,B00000000,
  B11000000,B01111000,B00000000,B00000000,
  B11100000,B00111000,B00000000,B00000000,
  B11100000,B00011100,B00000000,B00000000,
  B11100000,B00001110,B00000000,B00000011,
  B11100000,B00001111,B10000000,B00001111,
  B11100000,B00000011,B11110000,B01111111,
  B01110000,B00000001,B11111111,B11111111,
  B01110000,B00000000,B01111111,B11111110,
  B00111000,B00000000,B00011111,B11011110,
  B00111100,B00000000,B00000000,B00011100,
  B00011110,B00000000,B00000000,B00111100,
  B00001111,B00000000,B00000000,B01111000,
  B00001111,B10000000,B00000001,B11110000,
  B00000011,B11100000,B00000111,B11100000,
  B00000001,B11111111,B01111111,B10000000,
  B00000000,B01111111,B11111111,B00000000,
  B00000000,B00001111,B11111000,B00000000
};

static const unsigned char halfCloudLogo_bmp[] PROGMEM = {
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00001100,B00000000,
  B00000000,B00000000,B00001100,B00000000,
  B00000000,B00000010,B00001100,B00001000,
  B00000000,B00000011,B00000100,B00011000,
  B00000000,B00000011,B00000000,B00111000,
  B00000000,B00111111,B00111111,B00110000,
  B00000000,B11111111,B01111111,B10000000,
  B00000001,B11111111,B11110001,B11000000,
  B00000011,B10000001,B11100000,B11100000,
  B00000111,B00000000,B11100000,B11100000,
  B00000110,B00000000,B01111100,B01101111,
  B00001110,B00000000,B11111111,B11100110,
  B00111100,B00000001,B10000011,B11100000,
  B01110000,B00000001,B00000001,B11000000,
  B11100000,B00000000,B00000001,B11000000,
  B11000000,B00000000,B00000001,B11110000,
  B11000000,B00000000,B00000000,B11111000,
  B11000000,B00000000,B00000001,B11011000,
  B11000000,B00000000,B00000001,B11001000,
  B11100000,B00000000,B00000011,B10000000,
  B01111111,B11111111,B11111111,B00000000,
  B00111111,B11111111,B11111110,B00000000,
  B00001111,B11111111,B11111000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000
};

static const unsigned char cloudLogo_bmp[] PROGMEM = {
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00011111,B11100000,B00000000,
  B00000000,B01111111,B11111000,B00000000,
  B00000000,B11110000,B00111100,B00000000,
  B00000001,B11000000,B00001110,B00000000,
  B00000001,B10000000,B00000111,B00000000,
  B00000011,B10000000,B00000111,B11100000,
  B00000011,B00000000,B00001111,B11111000,
  B00001111,B00000000,B00011110,B00111100,
  B00111110,B00000000,B00111000,B00001110,
  B01111000,B00000000,B00000000,B00000110,
  B01100000,B00000000,B00000000,B00000111,
  B11100000,B00000000,B00000000,B00000011,
  B11000000,B00000000,B00000000,B00000011,
  B11000000,B00000000,B00000000,B00000011,
  B11000000,B00000000,B00000000,B00000011,
  B11000000,B00000000,B00000000,B00000111,
  B11100000,B00000000,B00000000,B00001110,
  B01110000,B00000000,B00000000,B00011110,
  B00111111,B11111111,B11111111,B11111100,
  B00011111,B11111111,B11111111,B11110000,
  B00000001,B11111111,B11111111,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000
};

static const unsigned char mistLogo_bmp[] PROGMEM = {
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B01100010,B00000000,B00000000,
  B00000000,B00100010,B00000000,B00000000,
  B00000110,B00011100,B00110000,B00000000,
  B00000011,B01111111,B01100000,B00000000,
  B00000001,B11000001,B10000000,B00000000,
  B00000001,B00000000,B01000000,B00000000,
  B00011011,B00000000,B01101100,B00000000,
  B00001010,B00000000,B00101000,B00000000,
  B00000010,B00000000,B00100000,B00000000,
  B00000010,B00001111,B00100000,B00000000,
  B00001010,B00111111,B11101000,B00000000,
  B00011011,B01100000,B01101100,B00000000,
  B00000001,B11000000,B00110000,B00000000,
  B00000001,B10000000,B00011111,B00000000,
  B00000011,B10000000,B00011111,B10000000,
  B00001111,B00000000,B00110000,B11000000,
  B00011000,B00000000,B00100000,B01100000,
  B00011000,B00000000,B00100000,B01100000,
  B00010000,B00000000,B00000000,B01100000,
  B00011111,B11111111,B11000011,B11000000,
  B00111111,B11111111,B11100111,B11011110,
  B00111111,B11111111,B11111111,B11011110,
  B00011111,B11111111,B11111111,B10001100,
  B01111110,B00111111,B11111111,B00011110,
  B11111111,B01111111,B11111111,B10111111,
  B11111111,B01111111,B11111111,B10111111,
  B01111110,B01111111,B11111111,B00011110,
  B00000111,B11111111,B00001111,B11100000,
  B00001111,B11111111,B10011111,B11110000,
  B00001111,B11111111,B10011111,B11110000,
  B00000111,B11111111,B00001111,B11100000
};

static const unsigned char drizzleLogo_bmp[] PROGMEM = {
  B00000000,B00000011,B00000000,B00000000,
  B00000000,B00011111,B11100000,B00000000,
  B00000000,B01111111,B11111000,B00000000,
  B00000000,B11111000,B00111100,B00000000,
  B00000001,B11100000,B00011110,B00000000,
  B00000001,B11000000,B00000111,B00000000,
  B00000011,B10000000,B00000111,B11100000,
  B00000011,B00000000,B00001111,B11111000,
  B00001111,B00000000,B00011111,B01111100,
  B00111111,B00000000,B00111000,B00011110,
  B01111000,B00000000,B00010000,B00001110,
  B01110000,B00000000,B00000000,B00000111,
  B11100000,B00000000,B00000000,B00000111,
  B11000000,B00000000,B00000000,B00000011,
  B11000000,B00000000,B00000000,B00000011,
  B11000000,B00000000,B00000000,B00000111,
  B11100000,B00000000,B00000000,B00000111,
  B11100000,B00000000,B00000000,B00001110,
  B01110000,B00000000,B00000000,B00011110,
  B00111111,B11111111,B11111111,B11111100,
  B00011111,B11111111,B11111111,B11110000,
  B00000011,B11111111,B11111111,B10000000,
  B00000011,B10000001,B10000001,B11000000,
  B00000011,B11000011,B11000011,B11000000,
  B00000111,B11000111,B11100011,B11100000,
  B00000110,B11100111,B11100111,B01100000,
  B00001110,B01101110,B01110110,B01110000,
  B00001100,B01111110,B01111110,B00110000,
  B00001110,B01101110,B01110110,B01110000,
  B00001111,B11100111,B11100111,B11110000,
  B00000111,B11000011,B11000011,B11100000,
  B00000001,B00000001,B10000000,B10000000
};

static const unsigned char rainLogo_bmp[] PROGMEM = {
  B00000000,B00001111,B11110000,B00000000,
  B00000000,B00111100,B00111100,B00000000,
  B00000000,B01110000,B00001110,B00000000,
  B00000000,B11100000,B00000111,B00000000,
  B00000011,B11000000,B00000011,B11000000,
  B00011111,B11100000,B00000001,B11111000,
  B00111100,B11110000,B00000001,B11111100,
  B01110000,B00011000,B00000001,B11001110,
  B01100000,B00001000,B00000000,B11000110,
  B11000000,B00000000,B00000000,B11000011,
  B11000000,B00000000,B00000000,B00000011,
  B11000000,B00000000,B00000000,B00000011,
  B11000000,B00000000,B00000000,B00000011,
  B11000000,B00000000,B00100000,B00000011,
  B11000000,B00000000,B00110000,B00000011,
  B01100000,B00000000,B00110000,B00000110,
  B01110000,B00000000,B00011000,B00001110,
  B00111100,B00000000,B00001111,B00111100,
  B00001111,B11111111,B11111111,B11110000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00011000,B01100011,B00000000,
  B00000000,B00011000,B11000011,B00000000,
  B00000000,B00110000,B11000110,B00000000,
  B00000000,B01110001,B10001110,B00000000,
  B00000000,B01100000,B00001100,B00000000,
  B00000000,B01000000,B00011000,B00000000,
  B00000000,B00000110,B00010000,B00000000,
  B00000000,B00001100,B00000000,B00000000,
  B00000011,B00011100,B01000000,B00000000,
  B00000110,B00011000,B11000000,B00000000,
  B00000110,B00110000,B10000000,B00000000
};

static const unsigned char thunderstormLogo_bmp[] PROGMEM = {
  B00000000,B00001111,B11100000,B00000000,
  B00000000,B00111111,B11111000,B00000000,
  B00000000,B11111000,B00111100,B00000000,
  B00000000,B11100000,B00011110,B00000000,
  B00000001,B11000000,B00000110,B00000000,
  B00000011,B10000000,B00000111,B11000000,
  B00000011,B00000000,B00001111,B11111000,
  B00001111,B00000000,B00011111,B11111100,
  B00011111,B00000000,B00111000,B00001110,
  B00111000,B00000000,B00010000,B00000110,
  B01110000,B00000001,B11110000,B00000111,
  B11100000,B00000011,B11111000,B00000011,
  B11000000,B00000011,B11111000,B00000011,
  B11000000,B00000111,B01110000,B00000011,
  B11000000,B00000110,B01100000,B00000011,
  B11100000,B00001110,B11110000,B00000111,
  B11100000,B00001100,B11111000,B00001110,
  B01110000,B00011100,B11111000,B00011110,
  B00111111,B11111110,B01111111,B11111100,
  B00011111,B11111111,B11111111,B11110000,
  B00000111,B11111111,B11111111,B10000000,
  B00000110,B00001111,B10011000,B01100000,
  B00000110,B00001111,B00011000,B01100000,
  B00000110,B00001111,B00011000,B01100000,
  B00000110,B11011111,B11011011,B01100000,
  B00000110,B11011101,B11011011,B01100000,
  B00000110,B11011001,B11011011,B01100000,
  B00000110,B11001001,B11011011,B01100000,
  B00000110,B00001100,B00011000,B01100000,
  B00000110,B11001101,B11011011,B01100000,
  B00000110,B11001101,B11011011,B01100000,
  B00000110,B11001100,B11010011,B00100000
};

static const unsigned char thunderLogo_bmp[] PROGMEM = {
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000111,B11000000,B00000000,
  B00000000,B00111111,B11110000,B00000000,
  B00000000,B01111100,B11111000,B00000000,
  B00000000,B11100000,B00011100,B00000000,
  B00000001,B11000000,B00001110,B00000000,
  B00000011,B10000000,B00000111,B00000000,
  B00000011,B00000000,B00000111,B11110000,
  B00000011,B00000000,B00001111,B11111000,
  B00011111,B00000000,B00011100,B00011100,
  B00111110,B00000000,B00011000,B00001110,
  B01110000,B00000000,B00000000,B00000111,
  B11100000,B00000001,B11111000,B00000011,
  B11000000,B00000011,B11111000,B00000011,
  B11000000,B00000011,B00110000,B00000011,
  B11000000,B00000111,B01110000,B00000011,
  B11000000,B00000110,B01100000,B00000011,
  B11100000,B00001110,B11111000,B00000111,
  B01100000,B00001100,B11111000,B00001110,
  B01111000,B00011100,B00110000,B00011100,
  B00111111,B11111111,B01111111,B11111000,
  B00001111,B11111111,B11111111,B11110000,
  B00000000,B00000111,B11000000,B00000000,
  B00000000,B00001111,B10000000,B00000000,
  B00000000,B00001111,B00000000,B00000000,
  B00000000,B00001110,B00000000,B00000000,
  B00000000,B00011100,B00000000,B00000000,
  B00000000,B00011000,B00000000,B00000000,
  B00000000,B00010000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000
};

static const unsigned char windLogo_bmp[] PROGMEM = {
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000011,B11100000,
  B00000000,B00000000,B00001111,B11111000,
  B00000000,B00000000,B00011111,B11111100,
  B00000000,B00000000,B00111100,B00011110,
  B00000000,B00000000,B00111000,B00001110,
  B00000000,B00000000,B01110000,B00000111,
  B00000000,B00000000,B01110000,B00000111,
  B00000000,B00000000,B01100000,B00000011,
  B00000000,B00000000,B01100000,B00000011,
  B00000000,B00000000,B00000000,B00000111,
  B00000000,B00000000,B00000000,B00000111,
  B00000000,B00000000,B00000000,B00001110,
  B00000000,B00000000,B00000000,B00011110,
  B00111111,B11111111,B11111111,B11111100,
  B01111111,B11111111,B11111111,B11110000,
  B00111111,B11111111,B11111111,B11000000,
  B00000000,B00000000,B00000000,B00000000,
  B11111111,B11111111,B11110000,B00000000,
  B11111111,B11111111,B11111100,B00000000,
  B11111111,B11111111,B11111110,B00000000,
  B00000000,B00000000,B00001111,B00000000,
  B00000000,B00000000,B00000111,B00000000,
  B00000000,B00001100,B00000011,B00000000,
  B00000000,B00001110,B00000011,B10000000,
  B00000000,B00001110,B00000011,B00000000,
  B00000000,B00001110,B00000111,B00000000,
  B00000000,B00000111,B00001111,B00000000,
  B00000000,B00000111,B11111110,B00000000,
  B00000000,B00000011,B11111100,B00000000,
  B00000000,B00000000,B11110000,B00000000,
  B00000000,B00000000,B00000000,B00000000
};

static const unsigned char snowLogo_bmp[] PROGMEM = {
  B00000000,B00000001,B10000000,B00000000,
  B00000000,B00000001,B10000000,B00000000,
  B00000000,B00010001,B10001000,B00000000,
  B00000000,B00111001,B10011100,B00000000,
  B00000001,B00011101,B10111000,B10000000,
  B00000011,B00001111,B11110000,B11000000,
  B00000011,B10000111,B11100001,B11000000,
  B00000001,B10000001,B10000001,B10000000,
  B00111001,B10000001,B10000001,B10011100,
  B00011111,B10000001,B10000001,B11111000,
  B00001111,B10000001,B10000001,B11110000,
  B00000011,B11000111,B11100011,B11000000,
  B00011111,B11111110,B01111111,B11111000,
  B00111100,B00111000,B00111100,B00111100,
  B00100000,B00011000,B00011000,B00001100,
  B00000000,B00011000,B00011000,B00000000,
  B00000000,B00011000,B00011000,B00000000,
  B00110000,B00011000,B00011000,B00000100,
  B00111100,B00111100,B00111100,B00111100,
  B00011111,B11111110,B01111111,B11111000,
  B00000011,B11000111,B11100011,B11000000,
  B00001111,B10000001,B10000001,B11110000,
  B00011111,B10000001,B10000001,B11111000,
  B00111001,B10000001,B10000001,B10011100,
  B00000001,B10000001,B10000001,B10000000,
  B00000001,B10000111,B11100001,B11000000,
  B00000011,B00001111,B11110000,B11000000,
  B00000001,B00011101,B10111000,B10000000,
  B00000000,B00111001,B10011100,B00000000,
  B00000000,B00010001,B10001100,B00000000,
  B00000000,B00000001,B10000000,B00000000,
  B00000000,B00000001,B10000000,B00000000
};

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  dht.begin();

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.

  // Clear the buffer
  display.clearDisplay();

  while(!Serial) { }

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("connected");

  timeClient.begin();

  setBootText();
}

int timeSinceLastRead = 0;

void loop() {
  timeClient.update();
  if(methodTimeCounter < 5000) {
  measureRoom();
  } else if(methodTimeCounter >= 5000 && methodTimeCounter < 10000) {
  getCurrentWeather();
  } else {
    methodTimeCounter = 0;
  }
}

void setBootText() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(F("Device started"));
  display.println(F("---------------------"));
  display.println(F("DHT started"));
  display.println(F("---------------------"));
  display.println(F("WiFi connected"));
  display.println(F("---------------------"));

  display.display();
  delay(2000);
}

void showTopBar() {
  display.drawBitmap(0, 0, wifiLogo_bmp, LOGO_WIDTH, LOGO_HEIGHT, WHITE);
  display.setCursor(48, 0);
  display.print(timeClient.getHours());
  display.print(":");
  if(timeClient.getMinutes() < 10) {
    display.print(0);
  }
  display.print(timeClient.getMinutes());
}

void measureRoom() {
  // Report every 2 seconds.
  if(timeSinceLastRead > 2000) {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
//    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      display.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }

    // Compute heat index in Fahrenheit (the default)
//    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    setMeasurementText(h, t, hic);

    timeSinceLastRead = 0;
  }
  delay(100);
  timeSinceLastRead += 100;
  methodTimeCounter += 100;
}

void setMeasurementText(float h, float t, float hic) {
  display.clearDisplay();

  if(WiFi.status() == WL_CONNECTED) {
    showTopBar();
  }
  display.drawBitmap(10, 18, tempLogo_bmp, LOGO_WIDTH, LOGO_HEIGHT, WHITE);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(30,18);
  display.print(t);
  display.print((char)247);
  display.print("C");
  
  display.drawBitmap(10, 37, humidLogo_bmp, LOGO_WIDTH, LOGO_HEIGHT, WHITE);
  display.setCursor(30, 37);
  display.print(h);
  display.println("%");
  display.println("");
  display.setCursor(5, 57);
  display.setTextSize(1);
  display.print("Heat index: ");
  display.print(hic);
  display.print((char)247);
  display.println("C");
  display.display();
}

void getCurrentWeather() {
  if(WiFi.status() == WL_CONNECTED) {
    WiFiConnected = true;
    if(countdown == 0 || countdown < 0) { //refresh only every 10 minutes
      HTTPClient http;

      http.begin("http://api.openweathermap.org/data/2.5/weather?q=Dordrecht,nl&APPID=fbef08a7ca0b188cf2b9fb3335b0badf");
      int httpCode = http.GET();

        Stream& response = http.getStream();

        DynamicJsonDocument doc(2048);

        DeserializationError err = deserializeJson(doc, response);

        if(err) {
          display.print("err");
        }

        weather_main = doc["weather"][0]["main"];
        weather_main_desc = doc["weather"][0]["description"];
        temp = doc["main"]["temp"];
        humid = doc["main"]["humidity"];
        windspeed = doc["wind"]["speed"];

      http.end(); //Close connection
      countdown = 600000;
    }
  } else {
    WiFiConnected = false;
  }
  setCurrentWeatherText();
}

void setCurrentWeatherText() {
  display.clearDisplay();

  if(WiFiConnected) {
    showTopBar();
  }
  
  display.drawBitmap(10, 16, getWeatherLogo(), LOGO_WIDTH_WEATHER, LOGO_HEIGHT_WEATHER, WHITE);
  display.drawBitmap(50, 10, tempLogo_bmp, LOGO_WIDTH, LOGO_HEIGHT, WHITE);
  display.setCursor(70, 14);
  display.setTextSize(1);
  display.print(convertTemp(temp));
  display.print((char)247);
  display.println("C");
  display.drawBitmap(50, 28, humidLogo_bmp, LOGO_WIDTH, LOGO_HEIGHT, WHITE);
  display.setCursor(70, 32);
  display.print(humid);
  display.println("%");
  display.drawBitmap(50, 47, windLogoSmall_bmp, LOGO_WIDTH, LOGO_HEIGHT, WHITE);
  display.setCursor(70, 52);
  display.print(windspeed);
  display.println("km/h");
  
  display.display();
  methodTimeCounter += 5000;
  if(countdown > 0)
  {
    countdown -= 5000;
  }

  delay(5000);
}

float convertTemp(float t) 
{
  float celsius = temp - 273.15;
  return celsius;
}

const unsigned char* getWeatherLogo() {
  if(strcmp(weather_main, "Thunderstorm") == 0) {
    if(strcmp(weather_main_desc, "thunderstorm with light rain") == 0) {
      return thunderLogo_bmp;
    } else {
      return thunderstormLogo_bmp;
    }
  }
  else if(strcmp(weather_main, "Drizzle") == 0) {
    return drizzleLogo_bmp;
  }
  else if(strcmp(weather_main, "Rain") == 0) {
    return rainLogo_bmp;
  }
  else if(strcmp(weather_main, "Snow") == 0) {
    return snowLogo_bmp;
  }
  else if(strcmp(weather_main, "Mist") == 0) {
    return mistLogo_bmp;
  }
  else if(strcmp(weather_main, "Smoke") == 0) {
    return mistLogo_bmp;
  }
  else if(strcmp(weather_main, "Haze") == 0) {
    return mistLogo_bmp;
  }
  else if(strcmp(weather_main, "Mist") == 0) {
    return mistLogo_bmp;
  }
  else if(strcmp(weather_main, "Fog") == 0) {
    return mistLogo_bmp;
  }
  else if(strcmp(weather_main, "Sand") == 0) {
    return mistLogo_bmp;
  }
  else if(strcmp(weather_main, "Ash") == 0) {
    return mistLogo_bmp;
  }
  else if(strcmp(weather_main, "Squall") == 0) {
    return mistLogo_bmp;
  }
  else if(strcmp(weather_main, "Tornado") == 0) {
    return mistLogo_bmp;
  }
  else if(strcmp(weather_main, "Clear") == 0) {
    if(timeClient.getHours() <= 6 || timeClient.getHours() >= 18) {
      return moonLogo_bmp;
    } else {
      return sunLogo_bmp;
    }
  }
  else if(strcmp(weather_main, "Clouds") == 0) {
    if(strcmp(weather_main_desc, "few clouds: 11-25%")) {
      return halfCloudLogo_bmp;
    } else {
      return cloudLogo_bmp;
    }
  } else {
    return sunLogo_bmp;
  }
}
