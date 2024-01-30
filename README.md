# RPN_Calculator
A Graphical RPN (Reverse Polish notation) Calculator that runs on the `esp32` using a 4x6 keypad with a `ST7789` 240x240 TFT screen 

Calculator includes:
```
- Shift and Alpha modyfiers 
- Addition, subtraction, multiplicaton, division
- To the power of any
- The root of any kind
- sin, cos, tan, cot
- Up to 100+ stacks
- Cloning the latest stack
- Moving stacks up and down
- Executing the same operation multiple times thanks to the repeat function
- Changing the positivity/negativity of the stacks sign
- Scientific notation
- Some physical, chemical and mathematical constants
- Some mathematical and physical formulas
- Graphing quadratic and polynomial functions
- Moving around the graph
- Zooming into and out of the graph with adaptive scaling
```

To be implemented:
```
- Other graphing functions like exponential functions
- More constants and formulas
- Add exact mode (for exaple 1/4 instead of 0.25)
```

## Hardware 
Tested on df robot DFR0649 and esp32 nodemcu-32s buttons and everything soldered by hand
Hardware Connections:
```
(Screen chip select) TFT_CS = pin 5
(Screen, SD card data/command) TFT_DC = pin 16
(Screen reset) TFT_RST = pin 2
(SD card chip select) (SD)SC = pin 17
```
keypad colums - `pin 14`, `pin 25`, `pin 26`, `pin 27` in order
keypad rows - `pin 33`, `pin 22`, `pin 21`, `pin 4`, `pin 13`, `pin 12` in order

## Calculator Manual

Button layout:
```
{'W', 'G', '^', 'D'}
{'X', 'L', 'v', '-'}
{'F', '7', '8', '9'}
{'S', '4', '5', '6'}
{'A', '1', '2', '3'}
{'J', '.', '0', 'E'}
```
### Basic operations:
While in insert mode type out the number on the `'0'` to `'9'` and `'.'` keys.
To push the number to the stack press the `'E'` key.

To change operating mode press:
```
'F' for function
'S' for Shift
'A' for Alpha
```
To return to Insert mode press the modyfier button again.
The active mode is indicated in the lower left corner of the screen.
 
While in shift mode press:
```
'9' for Addition
'6' for Subtraction
'3' for Multiplication
'E' for Division
```
this will merge the two layers at the bottom to one with the operation applied.
MANUAL NOT COMPLETE


also my programs that run on the same hardware:
- https://github.com/GreenSlimeStudios/KarpOS_ST7789
- https://github.com/GreenSlimeStudios/ultimate_tic_tac_toe_esp32