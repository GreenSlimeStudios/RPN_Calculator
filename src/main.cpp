#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>
#include <vector>
#include <bits/stdc++.h>

const int SCREEN_HEIGHT = 64;
const int SCREEN_WIDTH = 128;
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const byte ROWS = 4; // four rows
const byte COLS = 4; // three columns
char keys[ROWS][COLS] = {
    {'F', '1', '2', '3'},
    {'S', '4', '5', '6'},
    {'A', '7', '8', '9'},
    {'J', '.', '0', 'E'}};
byte colPins[ROWS] = {26, 18, 19, 23}; // connect to the row pinouts of the keypad
byte rowPins[COLS] = {5, 16, 33, 17};  // connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

enum MODE
{
  INSERT,
  SHIFT,
  ALPHA,
  FUNCTION,
};

MODE mode = MODE::INSERT;
std::vector<long double> stack = {};
std::string current = "";

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
void print_stack()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  int skip = 0;
  if (stack.size() > 4)
  {
    skip = stack.size() - 4;
  }

  for (int i = skip; i < stack.size(); ++i)
  {
    display.print(char(65 + i));
    display.print(" ");
    display.println(std::to_string(stack[i]).c_str());
    display.drawLine(0, display.getCursorY() + 1, 128, display.getCursorY() + 1, 1);
    display.setCursor(0, display.getCursorY() + 3);
  }
  display.display();
}
void push_to_stack()
{
  if (current != "")
  {
    stack.push_back(std::stold(current));
    current = "";
  }
}

void loop()
{
  // Serial.print("MODE: ");
  // Serial.println(mode);
  char key = keypad.getKey();

  if (key)
  {
    if (key == 'F')
    {
      if (mode == MODE::FUNCTION)
        mode = MODE::INSERT;
      else
        mode = MODE::FUNCTION;
    }
    else if (key == 'S')
    {
      if (mode == MODE::SHIFT)
        mode = MODE::INSERT;
      else
        mode = MODE::SHIFT;
    }
    else if (key == 'A')
    {
      if (mode == MODE::ALPHA)
        mode = MODE::INSERT;
      else
        mode = MODE::ALPHA;
    }
    else if (key == 'E' && mode == MODE::INSERT)
    {
      push_to_stack();
      print_stack();
    }
    else
    {
      if (mode == MODE::INSERT)
      {
        print_stack();
        current += key;
        display.print(current.c_str());
        // display.print(key);
        Serial.println(key);
        display.display();
      }
      if (mode == MODE::ALPHA)
      {
        switch (key)
        {
        case 'E':
          if (current != "")
          {

            current = "";
            print_stack();
            mode = MODE::INSERT;
          }
          else
          {
            stack.pop_back();
            print_stack();
            mode = MODE::INSERT;
          }
          break;

        default:
          break;
        }
      }
      else if (mode == MODE::SHIFT)
      {
        if (key == '3' || key == '6' || key == '9' || key == 'E')
        {
          push_to_stack();
          long double result;
          if (stack.size() < 2)
          {
            mode = MODE::INSERT;
            return;
          }
          switch (key)
          {
          case '3':
            result = stack[stack.size() - 2] + stack[stack.size() - 1];
            break;
          case '6':
            result = stack[stack.size() - 2] - stack[stack.size() - 1];
            break;
          case '9':
            result = stack[stack.size() - 2] * stack[stack.size() - 1];
            break;
          case 'E':
            result = stack[stack.size() - 2] / stack[stack.size() - 1];
            break;
          default:
            result = 0;
            break;
          }
          stack.pop_back();
          stack.pop_back();
          stack.push_back(result);
          mode = MODE::INSERT;
          print_stack();
        }
      }
    }
  }
}