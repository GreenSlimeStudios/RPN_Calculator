#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

const int SCREEN_HEIGHT = 64;
const int SCREEN_WIDTH = 128;
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const byte ROWS = 4; // four rows
const byte COLS = 4; // three columns
char keys[ROWS][COLS] = {
    {'F', '1', '2', '3'},
    {'N', '4', '5', '6'},
    {'H', '7', '8', '9'},
    {'J', '*', '0', '#'}};
byte colPins[ROWS] = {26, 18, 19, 23}; // connect to the row pinouts of the keypad
byte rowPins[COLS] = {5, 16, 33, 17};  // connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup()
{
  Serial.begin(921600);
  // put your setup code here, to run once:
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
    {
    }
  }
  Serial.println(F("wha the heck"));
  display.clearDisplay();
  display.setTextColor(1);
  // display.println("wha the heck");
  display.display();
}

void loop()
{
  char key = keypad.getKey();

  if (key)
  {
    display.print(key);
    Serial.println(key);
    display.display();
  }
}