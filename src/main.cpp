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

void setup_tictactoe();

const byte ROWS = 6; // four rows
const byte COLS = 4; // three columns
char keys[ROWS][COLS] = {
    {'W', 'G', '^', 'D'},
    {'X', 'L', 'v', '-'},
    {'F', '7', '8', '9'},
    {'S', '4', '5', '6'},
    {'A', '1', '2', '3'},
    {'J', '.', '0', 'E'}};
byte colPins[COLS] = {14, 25, 26, 27};        // connect to the row pinouts of the keypad
byte rowPins[ROWS] = {33, 22, 21, 4, 13, 12}; // connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
int repeat = 1;
char pre_key = 'E';
int index_offset = 1;

std::vector<long double> params = {};
std::string p_current = "";

enum MODE
{
  INSERT,
  SHIFT,
  ALPHA,
  FUNCTION,
  REPEAT_SET,
  CONST_SELECT,
  GRAPH,
  DISPLAY_GRAPH,
  SET_GRAPH_ZOOM,
  WZORY,
  // TICTACTOE,

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
void print_graph_select()
{
  display.fillScreen(ST77XX_BLACK);
  display.setCursor(0, 0);
  for (int i = 0; i < params.size(); ++i)
  {
    std::string num = std::to_string(params[i]);
    if (floor(params[i]) != params[i])
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
    display.print(num.c_str());
    if (i > 0)
    {
      display.print("X");
    }
    if (i > 1)
    {
      display.print("^");
      display.print(i);
    }
    if (i != params.size() - 1)
    {
      display.print("+");
    }
  }
  display.println();
  display.print(p_current.c_str());
}
void print_graph_zoom()
{
  display.fillRoundRect(10, 10, 230, 30, 5, ST77XX_BLUE);
  display.setCursor(15, 15);
  display.print(p_current.c_str());
}
float calc_func(float x)
{
  float out = 0;
  for (int i = 0; i < params.size(); ++i)
  {
    out += params[i] * pow(x, i);
  }
  return out;
}
float zoom = 10;
float x_offset = 0;
float y_offset = 0;
void print_graph()
{
  mode = MODE::DISPLAY_GRAPH;
  display.fillScreen(ST77XX_BLACK);
  display.drawLine(120 + x_offset, 0, 120 + x_offset, 240, ST77XX_WHITE);
  display.drawLine(0, 120 + y_offset, 240, 120 + y_offset, ST77XX_WHITE);
  int distance = 4;
  int long_lines = 1;
  int long_lines_mod = 40;
  for (int i = 1; i < (int)((240 - x_offset) / zoom) + 1; ++i)
  {
    long_lines = (int)(long_lines_mod / zoom);
    if (long_lines < 1)
    {
      long_lines = 1;
    }
    if (i % long_lines == 0)
    {
      display.drawLine(120 + i * zoom + x_offset, 0, 120 + i * zoom + x_offset, 240, 0x39C8);
    }
    distance = (i % 5 == 0) ? (7) : (3);
    display.drawLine(120 + i * zoom + x_offset, 120 - distance + y_offset, 120 + i * zoom + x_offset, 120 + distance + y_offset, ST77XX_WHITE);
  }
  for (int i = 1; i < (int)((240 + x_offset) / zoom) + 1; ++i)
  {
    long_lines = (int)(long_lines_mod / zoom);
    if (long_lines < 1)
    {
      long_lines = 1;
    }
    if (i % long_lines == 0)
    {
      display.drawLine(120 - i * zoom + x_offset, 0, 120 - i * zoom + x_offset, 240, 0x39C8);
    }
    distance = (i % 5 == 0) ? (7) : (3);
    display.drawLine(120 - i * zoom + x_offset, 120 - distance + y_offset, 120 - i * zoom + x_offset, 120 + distance + y_offset, ST77XX_WHITE);
  }
  for (int i = 1; i < (int)((240 - y_offset) / zoom) + 1; ++i)
  {
    long_lines = (int)(long_lines_mod / zoom);
    if (long_lines < 1)
    {
      long_lines = 1;
    }
    if (i % long_lines == 0)
    {
      display.drawLine(0, 120 + i * zoom + y_offset, 240, 120 + i * zoom + y_offset, 0x39C8);
    }
    distance = (i % 5 == 0) ? (7) : (3);
    display.drawLine(120 - distance + x_offset, 120 + i * zoom + y_offset, 120 + distance + x_offset, 120 + i * zoom + y_offset, ST77XX_WHITE);
  }
  for (int i = 1; i < (int)((240 + y_offset) / zoom) + 1; ++i)
  {
    long_lines = (int)(long_lines_mod / zoom);
    if (long_lines < 1)
    {
      long_lines = 1;
    }
    if (i % long_lines == 0)
    {
      display.drawLine(0, 120 - i * zoom + y_offset, 240, 120 - i * zoom + y_offset, 0x39C8);
    }
    distance = (i % 5 == 0) ? (7) : (3);
    display.drawLine(120 - distance + x_offset, 120 - i * zoom + y_offset, 120 + distance + x_offset, 120 - i * zoom + y_offset, ST77XX_WHITE);
  }
  for (int i = 1 - (int)x_offset * 10; i < 2400 - (int)x_offset * 10; ++i)
  {
    float x = ((float)i - 1200.0) / 10.0 / zoom;
    float y = calc_func(x) * zoom;
    display.drawPixel(i / 10 + x_offset, 120 - y + y_offset, ST77XX_RED);
  }
  Serial.println("finito");
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
long double factorial(long double x)
{
  long double res = 1;
  if (floorf(x) == x && x > 0)
  {
    for (int i = 1; i < x + 1; i++)
    {
      res *= i;
    }
  }
  else
  {
    stupid();
    return x;
  }
  return res;
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
int GCD(int x, int y)
{
  // Swaping variables
  if (y < x)
  {
    x = x + y;
    y = x - y;
    x = x - y;
  }
  int gcd = 1;
  for (int i = 1; i <= x; ++i)
  {
    if (x % i == 0 && y % i == 0)
    {
      gcd = i;
    }
  }
  return gcd;
}
void display_fraction_stack()
{
  p_current = std::to_string(stack.back());
  p_current.erase(p_current.find_last_not_of('0') + 1, std::string::npos);
  p_current.erase(p_current.find_last_not_of('.') + 1, std::string::npos);
  std::string result = "";
  int one, two;
  if (stack.back() != floorf(stack.back()))
  {
    std::string one_half = p_current.substr(0, p_current.find('.'));
    std::string two_half = p_current.substr(p_current.find('.') + 1, p_current.length());
    one = std::stoi(one_half + two_half);
    two = pow(10, two_half.length());
    int gcd = GCD(one, two);
    one = one / gcd;
    two = two / gcd;
    result = std::to_string(one) + "/" + std::to_string(two);
  }
  else
  {
    result = p_current;
  }
  print_stack();
  display.println(result.c_str());
  // display.println(std::to_string(one).c_str());
  // int width = (one > two) ? (std::to_string(one).length() * 11) : (std::to_string(two).length() * 11);
  // display.drawLine(0, display.getCursorY() + 1, width, display.getCursorY() + 1, ST77XX_WHITE);
  // display.setCursor(0, display.getCursorY() + 3);
  // display.println(std::to_string(two).c_str());
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
bool push_to_stack()
{
  if (check_input(&current))
  {
    stack.push_back(std::stold(current));
    current = "";
  }
  else if (current != "")
  {
    stupid();
    return false;
  }
  return true;
}
bool push_to_params()
{
  if (check_input(&p_current))
  {
    params.push_back(std::stold(p_current));
    p_current = "";
  }
  else if (p_current != "")
  {
    stupid();
    return false;
  }
  return true;
}
bool set_zoom()
{
  if (check_input(&p_current))
  {
    zoom = std::stold(p_current);
    p_current = "";
  }
  else if (p_current != "")
  {
    stupid();
    return false;
  }
  return true;
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
void print_const_set()
{
  display.fillScreen(ST77XX_BLACK);
  display.setCursor(5, 5);
  display.println("1:PI");
  display.setCursor(65, 5);
  display.println("2:e");
  display.setCursor(125, 5);
  display.println("3:me");
  display.setCursor(185, 5);
  display.println("4:p");
  display.setCursor(5, 30);
  display.println("5:G");
  display.setCursor(65, 30);
  display.println("6:ma");

  display.drawLine(60, 0, 60, 240, ST77XX_WHITE);
  display.drawLine(120, 0, 120, 240, ST77XX_WHITE);
  display.drawLine(180, 0, 180, 240, ST77XX_WHITE);
  display.drawLine(0, 25, 240, 25, ST77XX_WHITE);
  display.drawLine(0, 50, 240, 50, ST77XX_WHITE);
}
void print_wzory()
{
  display.fillScreen(ST77XX_BLACK);
  display.setCursor(0, 0);
  display.println("V=2{pi}r/T");
  display.println("B={mi0}nI/l - zwoj");
  display.println("Fe=qE");
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
        if (key == 'W')
        {
          setup_tictactoe();
        }
        if (key == 'X' || key == 'G' || key == 'W')
        {
          return;
        }
        else if (key == 'J')
        {
          if (push_to_stack())
          {
            display_fraction_stack();
          }
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
      else if (mode == MODE::FUNCTION)
      {
        switch (key)
        {
        case 'E':
          mode = MODE::INSERT;
          if (!push_to_stack)
          {
            break;
          }
          print_stack();
          if (stack.size() > 1)
          {
            if (floorf(stack.back()) == stack.back() && stack[stack.size() - 2] == floorf(stack[stack.size() - 2]))
            {
              int gcd = GCD((int)stack.back(), (int)stack[stack.size() - 2]);
              if ((int)stack.back() / gcd == 1)
              {
                display.println(std::to_string((int)stack[stack.size() - 2] / gcd).c_str());
              }
              else
              {
                display.println((std::to_string((int)stack[stack.size() - 2] / gcd) + "/" + std::to_string((int)stack.back() / gcd)).c_str());
              }
            }
          }
          break;

        default:
          break;
        }
      }
      else if (mode == MODE::ALPHA)
      {
        switch (key)
        {
        case 'J':
          if (push_to_stack())
          {
            mode = MODE::REPEAT_SET;
            print_repeat_set();
          }
          else
          {
            mode = MODE::INSERT;
            print_stack();
          }
          break;
        case '.':
          if (push_to_stack())
          {
            mode = MODE::CONST_SELECT;
            print_const_set();
          }
          else
          {
            mode = MODE::INSERT;
            print_stack();
          }
          break;
        case 'W':
          if (push_to_stack())
          {
            mode = MODE::WZORY;
            print_wzory();
          }
          else
          {
            mode = MODE::INSERT;
            print_stack();
          }
          break;
        case 'G':
          if (push_to_stack())
          {
            mode = MODE::GRAPH;
            print_graph_select();
          }
          else
          {
            mode = MODE::INSERT;
            print_stack();
          }
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
        case '7':
          for (int i = 0; i < repeat; ++i)
          {
            if (push_to_stack())
            {
              if (stack.size() > 0)
              {
                stack.push_back(stack[stack.size() - 1]);
              }
            }
            mode = MODE::INSERT;
            print_stack();
          }
          break;
        case '8':
          if (push_to_stack())
          {
            if (stack.size() > 0)
            {
              stack[stack.size() - 1] = -stack[stack.size() - 1];
            }
          }
          mode = MODE::INSERT;
          print_stack();
          break;
        case '5':
          if (push_to_stack())
          {
            if (stack.size() > 0)
            {
              stack[stack.size() - 1] = 1 / stack[stack.size() - 1];
            }
          }
          mode = MODE::INSERT;
          print_stack();
          break;
        case '3':
          if (push_to_stack())
          {
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
          }
          mode = MODE::INSERT;
          print_stack();
          break;
        case '4':
          if (push_to_stack())
          {

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
          }
          mode = MODE::INSERT;
          print_stack();
          break;
        case 'D':
          if (push_to_stack())
          {
            stack.clear();
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

        if (key == '7' || key == '8' || key == '4' || key == '5' || key == '1')
        {
          if (push_to_stack() == false)
          {
            mode = MODE::INSERT;
            print_stack();
            return;
          }
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
            result = factorial(stack.back());
            break;
          case '7':
            result = sin(stack[stack.size() - 1] * PI / 180);
            break;
          case '8':
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
          if (push_to_stack() == false)
          {
            mode = MODE::INSERT;
            print_stack();
            return;
          }
          for (int i = 0; i < repeat; ++i)
          {
            long double result;
            if (stack.size() < 2)
            {
              mode = MODE::INSERT;
              print_stack();
              return;
            }
            switch (key)
            {
            case '9':
              result = stack[stack.size() - 2] + stack[stack.size() - 1];
              break;
            case '6':
              result = stack[stack.size() - 2] - stack[stack.size() - 1];
              break;
            case '3':
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
      else if (mode == MODE::CONST_SELECT)
      {
        switch (key)
        {
        case 'E':
          mode = MODE::INSERT;
          print_stack();
          break;
        case '1':
          stack.push_back(PI);
          break;
        case '2':
          stack.push_back(0.00000000000000000016);
          break;
        case '3':
          stack.push_back(0.00000000000000000000000000000091);
          break;
        case '4':
          stack.push_back(0.00000000000000000000000167);
          break;
        case '5':
          stack.push_back(0.000000000066743);
          break;
        case '6':
          stack.push_back(0.0000000000000000000000000066446573357);
          break;

        default:
          break;
        }
        mode = MODE::INSERT;
        print_stack();
      }
      else if (mode == MODE::WZORY)
      {
        switch (key)
        {
        case '^':
          /* code */
          break;
        case 'v':
          break;

        default:
          mode = MODE::INSERT;
          print_stack();
          break;
        }
      }
      else if (mode == MODE::GRAPH)
      {
        if (key == '-')
        {
          if (p_current.substr(0, 1) == "-")
          {
            p_current = p_current.substr(1);
          }
          else
          {
            p_current = "-" + p_current;
          }
          print_graph_select();
          return;
        }
        else if (key == 'D')
        {
          if (p_current.length() > 0)
          {
            p_current = p_current.substr(0, p_current.length() - 1);
            print_graph_select();
          }
          return;
        }
        else if (key == '.')
        {
          if (p_current.find('.') == std::string::npos)
          {
            p_current += key;
            print_graph_select();
            Serial.println(key);
          }
        }
        else
        {
          if (std::find(std::begin(valid_chars), std::end(valid_chars), key) != std::end(valid_chars))
          {
            p_current += key;
          }
          print_graph_select();
          Serial.println(key);
        }

        switch (key)
        {
        case 'E':
          push_to_params();
          print_graph_select();
          break;
        case 'G':
          params.clear();
          p_current = "";
          print_graph_select();
          break;
        case 'W':
          mode = MODE::DISPLAY_GRAPH;
          Serial.println("printing graph");
          print_graph();
          break;
        default:
          break;
        }
      }
      else if (mode == MODE::DISPLAY_GRAPH)
      {
        // float vec = (zoom>10)?(zoom/2):(10);
        float vec = 10;
        float prop = 1;
        switch (key)
        {
        case 'E':
          mode = MODE::GRAPH;
          print_graph_select();
          break;
        case '9':
          prop = zoom / (zoom + zoom / 5);
          y_offset += y_offset * prop / 5;
          x_offset += x_offset * prop / 5;
          zoom += zoom / 5;
          print_graph();
          break;
        case '6':
          // if (zoom > )
          prop = zoom / (zoom + zoom / 5);
          y_offset -= y_offset * prop / 5;
          x_offset -= x_offset * prop / 5;
          zoom -= zoom / 5;
          print_graph();
          break;
        case '^':
          y_offset += vec;
          print_graph();
          break;
        case '8':
          y_offset -= vec;
          print_graph();
          break;
        case 'L':
          x_offset += vec;
          print_graph();
          break;
        case '-':
          x_offset -= vec;
          print_graph();
          break;
        case '7':
          x_offset = 0;
          y_offset = 0;
          print_graph();
          break;
        case '3':
          zoom = 10;
          print_graph();
          break;
        case 'v':
          mode = MODE::SET_GRAPH_ZOOM;
          p_current = std::to_string(zoom);
          p_current.erase(p_current.find_last_not_of('0') + 1, std::string::npos);
          p_current.erase(p_current.find_last_not_of('.') + 1, std::string::npos);
          print_graph_zoom();
          break;
        default:
          // print_graph();
          break;
        }
      }
      else if (mode == MODE::SET_GRAPH_ZOOM)
      {
        if (key == '.')
        {
          if (p_current.find('.') == std::string::npos)
          {
            p_current += key;
            print_graph_zoom();
            Serial.println(key);
          }
        }
        else if (std::find(std::begin(valid_chars), std::end(valid_chars), key) != std::end(valid_chars))
        {
          p_current += key;
          print_graph_zoom();
          Serial.println(key);
        }
        else if (key == 'D')
        {
          if (p_current.length() > 0)
          {
            p_current = p_current.substr(0, p_current.length() - 1);
            print_graph_zoom();
          }
          return;
        }
        else if (key == 'E')
        {
          set_zoom();
          mode = MODE::DISPLAY_GRAPH;
          print_graph();
        }
      }
      pre_key = key;
    }
  }
  // Serial.println(mode);
}

// #include <Arduino.h>
// #include <SPI.h>
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_ST7789.h>
// #include <Keypad.h>
// #include <vector>
// #include <bits/stdc++.h>

// const int TFT_CS = 5;
// const int TFT_DC = 16;
// const int TFT_RST = 2;
// const int SC = 17;

// const int SCREEN_HEIGHT = 240;
// const int SCREEN_WIDTH = 240;

const int PART_STEP = 24;
int segment = SCREEN_WIDTH / PART_STEP;
int segment_h = SCREEN_HEIGHT / PART_STEP;
int space = 1;
int next_pos = 10;

// Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// const byte ROWS = 6; // four rows
// const byte COLS = 4; // three columns
// char keys[ROWS][COLS] = {
//     {'A', '0', '1', '2'},
//     {'E', '3', '4', '5'},
//     {'I', '6', '7', '8'},
//     {'M', 'N', 'O', 'P'},
//     {'R', 'S', 'T', 'U'},
//     {'W', 'X', 'Y', 'Z'}};
// byte colPins[COLS] = {14, 25, 26, 27};        // connect to the row pinouts of the keypad
// byte rowPins[ROWS] = {33, 22, 21, 4, 13, 12}; // connect to the column pinouts of the keypad

// Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void print_splash_ttt()
{
  display.fillScreen(ST77XX_BLACK);
  display.println("ULTIMATE TIC TAC TOE");
}

void print_board();
enum Square
{
  FREE,
  P1,
  P2,
  DRAW,
};
struct GameState
{
  std::vector<std::vector<Square>> board;
  Square turn = P1;
  bool lock_button = false;

  GameState()
  {
    board.clear();
    for (int i; i < 9; ++i)
    {
      board.push_back({
          Square::FREE,
          Square::FREE,
          Square::FREE,
          Square::FREE,
          Square::FREE,
          Square::FREE,
          Square::FREE,
          Square::FREE,
          Square::FREE,
      });
    }
  }
};
GameState gs = GameState();
void looptictactoe();
void setup_tictactoe()
{
  // Serial.begin(921600);
  display.init(240, 240);
  display.setRotation(2);
  display.fillScreen(ST77XX_BLACK);
  display.drawRect(120, 120, 240, 240, ST77XX_BLACK);
  display.setTextColor(ST77XX_WHITE);
  display.setTextSize(2);
  print_splash_ttt();
  print_board();
  while (true)
  {
    looptictactoe();
  }
}
// void stupid()
// {
//   display.fillRect(20, 100, 200, 35, ST77XX_RED);
//   display.setCursor(25, 110);
//   display.print("SYNTAX ERROR !!!");
//   delay(500);
// }

Square check_game(std::vector<Square> v)
{
  // if (next_pos)
  bool playable = false;
  if (v[0] == v[3] && v[3] == v[6] && v[0] != Square::FREE)
  {
    return v[0];
  }
  else if (v[0] == v[4] && v[4] == v[8] && v[0] != Square::FREE)
  {
    return v[0];
  }
  else if (v[0] == v[1] && v[1] == v[2] && v[0] != Square::FREE)
  {
    return v[0];
  }
  else if (v[3] == v[4] && v[4] == v[5] && v[3] != Square::FREE)
  {
    return v[3];
  }
  else if (v[1] == v[4] && v[4] == v[7] && v[1] != Square::FREE)
  {
    return v[1];
  }
  else if (v[6] == v[7] && v[7] == v[8] && v[6] != Square::FREE)
  {
    return v[6];
  }
  else if (v[2] == v[5] && v[5] == v[8] && v[2] != Square::FREE)
  {
    return v[2];
  }
  else if (v[6] == v[4] && v[4] == v[2] && v[6] != Square::FREE)
  {
    return v[6];
  }
  for (int i = 0; i < 9; ++i)
  {
    if (v[i] == FREE)
    {
      playable = true;
      return FREE;
    }
  }

  return DRAW;
}

void draw_symbol(Square whos, int x, int y, bool is_verdict)
{
  // display.drawCircle(x + SCREEN_WIDTH / 28 / 2, y + SCREEN_HEIGHT / 28 / 2, 3, ST77XX_WHITE);
  if (whos == P1)
  {
    display.drawCircle(x, y, 5, (!is_verdict) ? ST77XX_RED : ST77XX_WHITE);
  }
  if (whos == P2)
  {
    display.drawLine(x - 4, y - 4, x + 4, y + 4, (!is_verdict) ? ST77XX_BLUE : ST77XX_WHITE);
    display.drawLine(x - 4, y + 4, x + 4, y - 4, (!is_verdict) ? ST77XX_BLUE : ST77XX_WHITE);
  }
  // display.drawPixel(x,y,ST77XX_CYAN);
}
void print_helper()
{
  Serial.println(next_pos);
  if (next_pos == 10)
  {
    for (int h = 0; h < 3; ++h)
    {
      for (int w = 0; w < 3; ++w)
      {
        // display.fillRoundRect(segment, segment, 6 * segment+1, 6 * segment+1,5, ST77XX_ORANGE);

        display.drawRoundRect(segment + 8 * segment * w, segment + 8 * segment * h, 6 * segment + 1, 6 * segment + 1, 5, ST77XX_ORANGE);
      }
    }
    return;
  }
  int h = floor((next_pos) / 3);
  int w = (next_pos) % 3;

  display.drawRoundRect(segment + 8 * segment * w, segment + 8 * segment * h, 6 * segment + 1, 6 * segment + 1, 5, ST77XX_ORANGE);
}

void print_small_board(int i, int j, bool is_verdict)
{
  display.drawLine(segment * 1 + segment * j * 8, segment_h * 3 + segment_h * i * 8, segment * 7 + segment * j * 8, segment_h * 3 + segment_h * i * 8, ST77XX_WHITE);
  display.drawLine(segment * 1 + segment * j * 8, segment_h * 5 + segment_h * i * 8, segment * 7 + segment * j * 8, segment_h * 5 + segment_h * i * 8, ST77XX_WHITE);
  display.drawLine(segment * 3 + segment * j * 8, segment_h * 1 + segment_h * i * 8, segment * 3 + segment * j * 8, segment_h * 7 + segment_h * i * 8, ST77XX_WHITE);
  display.drawLine(segment * 5 + segment * j * 8, segment_h * 1 + segment_h * i * 8, segment * 5 + segment * j * 8, segment_h * 7 + segment_h * i * 8, ST77XX_WHITE);
  for (int h = 0; h < 3; ++h)
  {
    for (int w = 0; w < 3; ++w)
    {
      draw_symbol(gs.board[i * 3 + j][h * 3 + w], SCREEN_WIDTH / PART_STEP + SCREEN_WIDTH / PART_STEP * j * 8 + SCREEN_WIDTH / PART_STEP * w * 2 + SCREEN_WIDTH / PART_STEP, segment_h + segment_h * i * 8 + segment_h * h * 2 + segment_h, is_verdict);
    }
  }
}
Square check_game_state()
{
  std::vector<Square> b_game;
  for (int i = 0; i < 9; ++i)
  {
    b_game.push_back(check_game(gs.board[i]));
  }
  return check_game(b_game);
}
void print_board()
{
  display.fillScreen(ST77XX_BLACK);
  if (gs.turn == P1)
  {
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ST77XX_RED);
  }
  else
  {
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ST77XX_BLUE);
  }
  print_helper();

  display.drawLine(SCREEN_WIDTH / 3, 0, SCREEN_WIDTH / 3, SCREEN_HEIGHT, ST77XX_WHITE);
  display.drawLine(SCREEN_WIDTH / 3 * 2, 0, SCREEN_WIDTH / 3 * 2, SCREEN_HEIGHT, ST77XX_WHITE);
  display.drawLine(0, SCREEN_WIDTH / 3, SCREEN_WIDTH, SCREEN_WIDTH / 3, ST77XX_WHITE);
  display.drawLine(0, SCREEN_WIDTH / 3 * 2, SCREEN_WIDTH, SCREEN_WIDTH / 3 * 2, ST77XX_WHITE);

  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      switch (check_game(gs.board[j * 3 + i]))
      {
      case P1:
        display.fillRoundRect(segment + 8 * segment * i, segment + 8 * segment * j, 6 * segment + 1, 6 * segment + 1, 5, ST77XX_RED);
        print_small_board(j, i, true);
        break;
      case P2:
        display.fillRoundRect(segment + 8 * segment * i, segment + 8 * segment * j, 6 * segment + 1, 6 * segment + 1, 5, ST77XX_BLUE);
        print_small_board(j, i, true);
        break;
      case DRAW:
        display.fillRoundRect(segment + 8 * segment * i, segment + 8 * segment * j, 6 * segment + 1, 6 * segment + 1, 5, ST77XX_CASET);
        print_small_board(j, i, true);

        break;

      default:
        print_small_board(j, i, false);
        break;
      }
    }
  }
}

void end_game(Square state)
{
  display.setCursor(5, 5);
  if (state == P1)
  {
    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 6, ST77XX_RED);
    display.setTextColor(ST77XX_WHITE);
    display.println("circle wins :3");
  }
  else if (state == P2)
  {
    // display.fillScreen(ST77XX_BLUE);
    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 6, ST77XX_BLUE);
    display.setTextColor(ST77XX_WHITE);
    display.println("cross wins >:3");
  }
  else
  {
    // display.fillScreen(ST77XX_BLACK);
    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 6, ST77XX_CASET);
    display.setTextColor(ST77XX_WHITE);
    display.println("draw >:[");
  }
  while (true)
  {
  }
}

void looptictactoe()
{
  int big_pos;

  char key = keypad.getKey();

  Square game_state = check_game_state();
  if (game_state != FREE)
  {
    end_game(game_state);
  }

  if (next_pos == 10 || check_game(gs.board[next_pos]) != FREE)
  {
    big_pos = 10;
    next_pos = 10;

    int playable = 0;
    int free_pos = 0;
    for (int i = 0; i < 9; ++i)
    {
      if (check_game(gs.board[i]) == Square::FREE)
      {
        playable++;
        free_pos = i;
      }
    }

    Serial.println(playable);
    if (playable == 1)
    {
      big_pos = free_pos;
      next_pos = free_pos;
      gs.lock_button = true;
      print_board();
      return;
    }

    print_board();
    while (true)
    {
      key = keypad.getKey();
      if (key == 'G' || key == '^' || key == 'D' || key == 'L' || key == 'v' || key == '-' || key == '7' || key == '8' || key == '9')
      {
        // int num = key - '0';
        int num = 0;
        switch (key)
        {
        case 'G':
          num = 0;
          break;
        case '^':
          num = 1;
          break;
        case 'D':
          num = 2;
          break;
        case 'L':
          num = 3;
          break;
        case 'v':
          num = 4;
          break;
        case '-':
          num = 5;
          break;
        case '7':
          num = 6;
          break;
        case '8':
          num = 7;
          break;
        case '9':
          num = 8;
          break;
        }
        if (check_game(gs.board[num]) == FREE)
        {
          big_pos = num;
          next_pos = num;
          gs.lock_button = true;
          print_board();
          break;
        }
      }
    }
  }
  else
  {
    big_pos = next_pos;
  }
  int cur = 0;
  if (gs.lock_button)
  {
    if (!key)
    {
      gs.lock_button = false;
    }
  }
  if (key && gs.lock_button == false)
  {
    switch (key)
    {
    case 'G':
      cur = 0;
      break;
    case '^':
      cur = 1;
      break;
    case 'D':
      cur = 2;
      break;
    case 'L':
      cur = 3;
      break;
    case 'v':
      cur = 4;
      break;
    case '-':
      cur = 5;
      break;
    case '7':
      cur = 6;
      break;
    case '8':
      cur = 7;
      break;
    case '9':
      cur = 8;
      break;

    default:
      break;
    }
    if (gs.board[big_pos][cur] == Square::FREE)
    {
      next_pos = cur;
      gs.board[big_pos][cur] = gs.turn;
      if (gs.turn == P1)
      {
        gs.turn = P2;
      }
      else
      {
        gs.turn = P1;
      }
    }
    print_board();
  }
}
