#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
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
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

const byte ROWS = 6; // four rows
const byte COLS = 4; // three columns
char keys[ROWS][COLS] = {
    {'X', 'X', '^', 'D'},
    {'X', 'X', 'v', '-'},
    {'F', '1', '2', '3'},
    {'S', '4', '5', '6'},
    {'A', '7', '8', '9'},
    {'J', '.', '0', 'E'}};
byte colPins[COLS] = {14, 25, 26, 27};        // connect to the row pinouts of the keypad
byte rowPins[ROWS] = {33, 22, 21, 4, 13, 12}; // connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
int repeat = 1;
char pre_key = 'E';
int index_offset = 1;

enum MODE
{
  INSERT,
  SHIFT,
  ALPHA,
  FUNCTION,
  REPEAT_SET,
  GRAPH,
};

MODE mode = MODE::INSERT;
std::vector<long double> stack = {};
std::string current = "";
std::string r_current = "";

void print_splash()
{
  display.fillScreen(ST77XX_BLACK);
  display.println("KARP CALCULATOR");
}
void setup()
{
  Serial.begin(921600);
  display.init(240, 240);
  display.setRotation(2);
  display.fillScreen(ST77XX_BLACK);
  display.drawRect(120, 120, 240, 240, ST77XX_BLACK);
  display.setTextColor(ST77XX_WHITE);
  display.setTextSize(2);
  print_splash();
}
void stupid()
{
  display.fillRect(20, 100, 200, 35, ST77XX_RED);
  display.setCursor(25, 110);
  display.print("SYNTAX ERROR !!!");
  delay(500);
}
void print_stack()
{
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

    long double nur = stack[i];
    int multiplyer = 0;
    std::string addition = "";

    if (nur < 0.0001 && nur > 0)
    {
      while (nur < 0.9999999)
      {
        nur *= 10;
        multiplyer -= 1;
      }
      addition = "x10^" + std::to_string(multiplyer);
    }
    else if (nur > 1000000)
    {
      while (nur >= 10)
      {
        nur /= 10;
        multiplyer += 1;
      }
      addition = "x10^" + std::to_string(multiplyer);
    }
    else if (nur < -1000000)
    {
      while (nur <= -10)
      {
        nur /= 10;
        multiplyer += 1;
      }
      addition = "x10^" + std::to_string(multiplyer);
    }
    else if (nur > -0.0001 && nur < 0)
    {
      while (nur > -0.999999999)
      {
        nur *= 10;
        multiplyer -= 1;
      }
      addition = "x10^" + std::to_string(multiplyer);
    }
    num = std::to_string(nur);

    if (floor(nur) != nur)
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
    num += addition;

    display.println(num.c_str());
    display.drawLine(0, display.getCursorY() + 1, SCREEN_WIDTH, display.getCursorY() + 1, ST77XX_WHITE);
    display.setCursor(0, display.getCursorY() + 3);
  }
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
}
char valid_chars[10] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
bool check_input(std::string *cur)
{
  if (*cur == "")
  {
    return false;
  }
  for (int i = 0; i < sizeof(valid_chars) / sizeof('1'); i++)
  {
    if (cur->find(valid_chars[i]) != std::string::npos)
    {
      return true;
    }
  }
  return false;
}
void push_to_stack()
{
  if (check_input(&current))
  {
    stack.push_back(std::stold(current));
    current = "";
  }
  else if (current != "")
  {
    stupid();
  }
}
void print_repeat_set()
{
  display.fillScreen(ST77XX_BLACK);
  display.setCursor(0, 0);
  display.println("SET REPEAT");
  display.drawLine(0, display.getCursorY() + 1, SCREEN_WIDTH, display.getCursorY() + 1, ST77XX_WHITE);
  display.setCursor(0, display.getCursorY() + 3);
  // display.setTextColor(ST77XX_BLUE);
  display.print(r_current.c_str());
  // display.setTextColor(ST77XX_WHITE);
}

void loop()
{
  // Serial.print("MODE: ");
  // Serial.println(mode);
  char key = keypad.getKey();

  if (key)
  {
    // if (key != '^' && key != 'v')
    // {
    //   pre_key = 'X';
    // }

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
        if (key == 'J' || key == 'X')
        {
          return;
        }
        else if (key == '^')
        {
          if (pre_key != '^' && pre_key != 'v')
          {
            index_offset = 1;
          }
          if (pre_key == 'v')
          {
            index_offset++;
          }
          if (index_offset > stack.size() - 1)
          {
            return;
          }
          push_to_stack();
          Serial.print("index: ");
          Serial.println(index_offset);
          long double temp = stack[stack.size() - 1 - index_offset];
          stack[stack.size() - 1 - index_offset] = stack[stack.size() - index_offset];
          stack[stack.size() - index_offset] = temp;
          index_offset += 1;
          pre_key = '^';
          Serial.print("index: ");
          Serial.println(index_offset);
          print_stack();
          return;
        }
        else if (key == 'v')
        {
          if (pre_key != 'v' && pre_key != '^')
          {
            index_offset = 0;
          }
          if (pre_key == '^')
          {
            index_offset--;
          }
          if (index_offset < 1)
          {
            return;
          }
          push_to_stack();
          long double temp = stack[stack.size() - index_offset];
          stack[stack.size() - index_offset] = stack[stack.size() - 1 - index_offset];
          stack[stack.size() - 1 - index_offset] = temp;

          print_stack();
          pre_key = 'v';
          index_offset -= 1;
          return;
        }
        else if (key == '-')
        {
          if (current.substr(0, 1) == "-")
          {
            current = current.substr(1);
          }
          else
          {
            current = "-" + current;
          }
          print_stack();
          return;
        }
        else if (key == 'D')
        {
          if (current.length() > 0)
          {
            current = current.substr(0, current.length() - 1);
            print_stack();
          }
          return;
        }
        else if (key == '.')
        {
          if (current.find('.') == std::string::npos)
          {
            current += key;
            print_stack();
            Serial.println(key);
          }
        }
        else
        {
          current += key;
          print_stack();
          Serial.println(key);
        }
      }
      else if (mode == MODE::ALPHA)
      {
        switch (key)
        {
        case 'J':
          push_to_stack();
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
        case '5':
          push_to_stack();
          if (stack.size() > 0)
          {
            stack[stack.size() - 1] = 1 / stack[stack.size() - 1];
          }
          mode = MODE::INSERT;
          print_stack();
          break;
        case '9':
          push_to_stack();
          if (stack.size() > 0)
          {
            if (repeat == 1)
            {
              stack.back() = pow(stack.back(), 2);
            }
            else
            {
              stack[stack.size() - 1] = pow(stack[stack.size() - 1], repeat);
            }
          }
          repeat = 1;
          mode = MODE::INSERT;
          print_stack();
          break;
        case '4':
          push_to_stack();
          if (stack.size() > 0)
          {
            if (stack.back() != 0)
            {
              if (repeat == 1)
              {
                stack[stack.size() - 1] = log10(stack[stack.size() - 1]);
              }
              else
              {
                stack[stack.size() - 1] = log2(stack[stack.size() - 1]) / log2(repeat);
              }
            }
            else
            {
              stupid();
            }
          }
          repeat = 1;
          mode = MODE::INSERT;
          print_stack();
          break;
        case 'D':
          push_to_stack();
          stack.clear();
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
            if (repeat == 1)
            {
              result = sqrt(stack[stack.size() - 1]);
            }
            else
            {
              result = pow(stack.back(), (1.0 / repeat));
            }
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
              print_stack();
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
              if (stack.back() != 0)
              {
                result = stack[stack.size() - 2] / stack[stack.size() - 1];
              }
              else
              {
                mode = MODE::INSERT;
                stupid();
                print_stack();
                return;
              }
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
        else if (key == 'D')
        {
          if (r_current.length() > 0)
          {
            r_current = r_current.substr(0, r_current.length() - 1);
            print_repeat_set();
          }
        }
        else if (key == '-')
        {
          if (r_current.substr(0, 1) == "-")
          {
            r_current = r_current.substr(1);
            print_repeat_set();
          }
          else
          {
            r_current = "-" + r_current;
            Serial.println(r_current.c_str());
            print_repeat_set();
          }
        }
        else if (key == 'E')
        {
          if (r_current != "" && r_current != "0")
          {
            repeat = std::stoi(r_current);
            r_current = "";
            mode = MODE::INSERT;
            print_stack();
          }
          else
          {
            repeat = 1;
            r_current = "";
            mode = MODE::INSERT;
            print_stack();
          }
        }
      }
    }
    pre_key = key;
  }
}