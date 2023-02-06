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

    std::string num;
    num = std::to_string(stack[i]);

    while (num[num.length() - 1] == '0' || num[num.length() - 1] == '.')
    {
      num = num.substr(0, num.size() - 1);
    }
    // }
    display.println(num.c_str());
    display.drawLine(0, display.getCursorY() + 1, 128, display.getCursorY() + 1, 1);
    display.setCursor(0, display.getCursorY() + 3);
  }
  // display.display();
  if (current != "")
  {
    display.print(char(65 + stack.size()));
    display.print(" ");
    display.print(current.c_str());
  }

  int cursorY = display.getCursorY();
  display.setCursor(0, 56);
  switch (mode)
  {
  case MODE::INSERT:
    display.print("I");
    break;
  case MODE::SHIFT:
    display.print("S");
    break;
  case MODE::ALPHA:
    display.print("A");
    break;
  case MODE::FUNCTION:
    display.print("F");
    break;

  default:
    break;
  }
  display.setCursor(0, cursorY);
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
      print_stack();
    }
    else if (key == 'S')
    {
      if (mode == MODE::SHIFT)
        mode = MODE::INSERT;
      else
        mode = MODE::SHIFT;
      print_stack();
    }
    else if (key == 'A')
    {
      if (mode == MODE::ALPHA)
        mode = MODE::INSERT;
      else
        mode = MODE::ALPHA;
      print_stack();
    }
    else if (key == 'E' && mode == MODE::INSERT)
    {
      push_to_stack();
      print_stack();
    }
    else if (key == 'J')
    {
    }
    else
    {
      if (mode == MODE::INSERT)
      {
        current += key;
        // display.print(char(65 + stack.size()));
        // display.print(" ");
        // display.print(current.c_str());
        print_stack();
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
          }
          else
          {
            if (stack.size() > 0)
            {
              stack.pop_back();
            }
          }
          break;
        case '1':
          push_to_stack();
          if (stack.size() > 0)
          {
            stack.push_back(stack[stack.size() - 1]);
          }
          break;
        default:
          break;
        }
        mode = MODE::INSERT;
        print_stack();
      }
      else if (mode == MODE::SHIFT)
      {
        if (key == '1' || key == '2' || key == '4' || key == '5')
        {
          push_to_stack();
          if (stack.size() < 1)
          {
            mode = MODE::INSERT;
            return;
          }
          long double result;
          switch (key)
          {
          case '1':
            result = sin(stack[stack.size() - 1] * PI / 180);
            break;
          case '2':
            result = cos(stack[stack.size() - 1] * PI / 180);
            break;
          case '4':
            result = tan(stack[stack.size() - 1] * PI / 180);
            break;
          case '5':
            result = sqrt(stack[stack.size() - 1]);
            break;
          default:
            break;
          }
          stack[stack.size() - 1] = result;
          mode = MODE::INSERT;
          print_stack();
        }
        else if (key == '3' || key == '6' || key == '9' || key == 'E')
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