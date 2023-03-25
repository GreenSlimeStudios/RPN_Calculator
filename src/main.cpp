#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ST7789.h>
#include <Keypad.h>
#include <vector>
#include <bits/stdc++.h>

const int TFT_CS = 5;
const int TFT_DC = 16;
const int TFT_RST = 2;
const int SC = 17;

const int SCREEN_HEIGHT = 240;
const int SCREEN_WIDTH = 240;
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

const byte ROWS = 6; // four rows
const byte COLS = 4; // three columns
char keys[ROWS][COLS] = {
    {'X', 'X', 'X', 'X'},
    {'X', 'X', 'X', 'X'},
    {'F', '1', '2', '3'},
    {'S', '4', '5', '6'},
    {'A', '7', '8', '9'},
    {'J', '.', '0', 'E'}};
byte colPins[COLS] = {14, 25, 26, 27};        // connect to the row pinouts of the keypad
byte rowPins[ROWS] = {33, 22, 21, 4, 13, 12}; // connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
int repeat = 1;

enum MODE
{
  INSERT,
  SHIFT,
  ALPHA,
  FUNCTION,
  REPEAT_SET,
};

MODE mode = MODE::INSERT;
std::vector<long double> stack = {};
std::string current = "";
std::string r_current = "";

void setup()
{
  Serial.begin(921600);
  Serial.println(F("wha the wha"));
  // put your setup code here, to run once:
  // if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  // {
  //   Serial.println(F("SSD1306 allocation failed"));
  //   for (;;)
  //   {
  //   }
  // }
  display.init(240, 240);
  display.setRotation(2);
  display.fillScreen(ST77XX_BLACK);
  display.drawRect(120, 120, 240, 240, ST77XX_BLACK);
  display.setTextColor(ST77XX_WHITE);
  display.setTextSize(2);
  // for (;;)
  // {
  // display.print("aaaaa");
  // Serial.println(F("wha the heck"));
  // }
  // display.clearDisplay();
  // display.setTextColor(1);
  // display.println("wha the heck");
  // display.display();
}
void print_stack()
{
  // display.clearDisplay();
  display.fillScreen(ST77XX_BLACK);
  display.setCursor(0, 0);
  int skip = 0;
  if (stack.size() > 8)
  {
    skip = stack.size() - 8;
  }

  for (int i = skip; i < stack.size(); ++i)
  {
    display.print(char(65 + i));
    display.print(" ");

    std::string num;
    num = std::to_string(stack[i]);

    if (floor(stack[i]) != stack[i])
    {
      while (num[num.length() - 1] == '0' || num[num.length() - 1] == '.')
      {
        num = num.substr(0, num.size() - 1);
      }
    }
    else
    {
      num = num.substr(0, num.size() - 7);
    }

    display.println(num.c_str());
    display.drawLine(0, display.getCursorY() + 1, SCREEN_WIDTH, display.getCursorY() + 1, ST77XX_WHITE);
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
  display.setCursor(0, SCREEN_HEIGHT - 16);
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
  if (repeat != 1)
  {
    display.print(" ");
    display.print(repeat);
    display.print("r");
  }
  display.setCursor(0, cursorY);
  // display.display();
}
void push_to_stack()
{
  if (current != "" && current != ".")
  {
    stack.push_back(std::stold(current));
    current = "";
  }
}
void print_repeat_set()
{
  // display.clearDisplay();
  display.fillScreen(ST77XX_BLACK);
  display.setCursor(0, 0);
  display.println("SET REPEAT");
  display.drawLine(0, display.getCursorY() + 1, SCREEN_WIDTH, display.getCursorY() + 1, ST77XX_WHITE);
  display.setCursor(0, display.getCursorY() + 3);
  // display.print("R: ");
  // display.setTextColor(0, 1);
  display.print(r_current.c_str());
  // display.setTextColor(1, 0);
  // display.display();
}

void loop()
{
  // Serial.print("MODE: ");
  // Serial.println(mode);
  char key = keypad.getKey();

  if (key)
  {
    if (key == 'F' && mode != MODE::REPEAT_SET)
    {
      if (mode == MODE::FUNCTION)
        mode = MODE::INSERT;
      else
        mode = MODE::FUNCTION;
      print_stack();
    }
    else if (key == 'S' && mode != MODE::REPEAT_SET)
    {
      if (mode == MODE::SHIFT)
        mode = MODE::INSERT;
      else
        mode = MODE::SHIFT;
      print_stack();
    }
    else if (key == 'A' && mode != MODE::REPEAT_SET)
    {
      if (mode == MODE::ALPHA)
      {
        mode = MODE::INSERT;
        repeat = 1;
      }
      else
        mode = MODE::ALPHA;
      print_stack();
    }
    else if (key == 'E' && mode == MODE::INSERT)
    {
      push_to_stack();
      print_stack();
    }
    // else if (key == 'J')
    // {
    // }
    else
    {
      if (mode == MODE::INSERT)
      {
        if (key == 'J')
        {
          return;
        }
        current += key;
        // display.print(char(65 + stack.size()));
        // display.print(" ");
        // display.print(current.c_str());
        print_stack();
        // display.print(key);
        Serial.println(key);
        // display.display();
      }
      else if (mode == MODE::ALPHA)
      {
        switch (key)
        {
        case 'J':
          mode = REPEAT_SET;
          print_repeat_set();
          break;
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
          mode = MODE::INSERT;
          print_stack();
          break;
        case '1':
          for (int i = 0; i < repeat; ++i)
          {
            push_to_stack();
            if (stack.size() > 0)
            {
              stack.push_back(stack[stack.size() - 1]);
            }
            mode = MODE::INSERT;
            print_stack();
          }
          break;
        case '2':
          push_to_stack();
          if (stack.size() > 0)
          {
            stack[stack.size() - 1] = -stack[stack.size() - 1];
          }
          mode = MODE::INSERT;
          print_stack();
          break;
        default:
          break;
        }
      }
      else if (mode == MODE::SHIFT)
      {
        if (key == '1' || key == '2' || key == '4' || key == '5')
        {
          // for (int i = 0; i < repeat; ++i)
          // {
          push_to_stack();
          if (stack.size() < 1)
          {
            mode = MODE::INSERT;
            print_stack();
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
          // }
        }
        else if (key == '3' || key == '6' || key == '9' || key == 'E')
        {
          for (int i = 0; i < repeat; ++i)
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
      else if (mode == REPEAT_SET)
      {
        if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9' || key == '0')
        {
          r_current += key;
          print_repeat_set();
        }
        if (key == 'E')
        {
          if (r_current != "" && r_current != "0")
          {
            repeat = std::stoi(r_current);
            r_current = "";
            mode = MODE::INSERT;
            print_stack();
          }
        }
      }
    }
  }
}