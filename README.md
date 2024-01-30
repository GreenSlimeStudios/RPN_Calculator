# RPN_Calculator
A Graphical RPN (Reverse Polish notation) Calculator that runs on the `esp32` using a 4x6 keypad with a `ST7789` 240x240 TFT screen 

Calculator includes:
```
- Shift and Alpha modyfiers 
- Addition, subtraction, multiplicaton, division
- To the power of any
- The root of any kind
- Logarithm's of any base
- sin, cos, tg, ctg
- Up to 100+ stacks
- Cloning the latest stack
- Moving stacks up and down
- Executing the same operation multiple times thanks to the repeat function
- Changing the positivity/negativity of the stacks sign
- Scientific notation
- Converting numbers to fractions
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
### Basic operations
While in insert mode type out the number on the `'0'` to `'9'` and `'.'` and `'-'` keys.
You can press the `'D'` key to delete the latest character.
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
This will merge the two layers at the bottom to one with the operation applied.
### Functions and actions
These are the keybinds for more advanced operations.
I will refer to the bottom layer as `x`.
I will refer to the second to bottom layer as `y`.
I will refer to the repeat value as `R`.
I will refer to the currently typed number as `current`.
If the action has a special behaviour when `R` is set, then it will be bolded at the end.
```
Shift '9' for adding x to y and *(add R times)*
Shift '6' for subtracting x from y and *(add R times)*
Shift '3' for multiplying x by y and *(add R times)*
Shift 'E' for pushing current to the stack
Insert 'D' to remove latest character from current
Alpha 'D' to clear the whole stack
Alpha 'E' to remove x (when there is no current) or current
Shift '7' for converting x to sin(x)
Shift '8' for converting x to cos(x)
Shift '4' for converting x to tg(x)
Shift '5' for converting x to sqrt(x) *(convert x to the root of the R's kind)*
Alpha '7' to duplicate the bottom layer *(duplicat R times)*
Alpha '8' to change the positivity/negativity of x's sign
Alpha '4' for converting x to log10(x) *(convert x to logR(x))*
Alpha '5' for inversing x (1/x)
Alpha '3' to convert x to x^2 *(convert x to x^R)*
Alpha '.' to change to the CONST SELECT mode
Alpha 'J' to go to the REPEAT CHANGING mode
Alpha 'A' to reset repeat value
Alpha 'G' to go to the GRAPH mode
Alpha 'W' to go to the FORMULA mode
Function 'E' to display x/y as a fraction
Insert 'J' to display x as a fraction
```
### The Repeat
Repeat is used to execute a thing many times or to change the behaviour of an action.
To set the repeat value press `Alpha 'J'` to go repeat set mode.
type in the value and confirm with `'E'`.

The newly set value will appear in the lower left corner of the screen next to the active mode indicator with a `R`r format.
To reset the repeat value press `'A'` two times (`Alpha 'A'`)

MANUAL NOT COMPLETE


also my programs that run on the same hardware:
- https://github.com/GreenSlimeStudios/KarpOS_ST7789
- https://github.com/GreenSlimeStudios/ultimate_tic_tac_toe_esp32