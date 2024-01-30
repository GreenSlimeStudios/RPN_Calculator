# RPN_Calculator
A Graphical RPN (Reverse Polish notation) Calculator that runs on the esp32 using a 4x6 keypad with a ST7789 240x240 TFT screen 

calculator includes:
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

to be implemented:
```
- Other graphing functions like exponential functions
- More constants and formulas
- Add exact mode (for exaple 1/4 instead of 0.25)
```

Button layout:
{'W', 'G', '^', 'D'}
{'X', 'L', 'v', '-'}
{'F', '7', '8', '9'}
{'S', '4', '5', '6'}
{'A', '1', '2', '3'}
{'J', '.', '0', 'E'}

Hardware Connections:
TFT_CS = pin 5
TFT_DC = pin 16
TFT_RST = pin 2
SC = pin 17 (for sd card)

keypad colums - pin 14, pin 25, pin 26, pin 27 in order
keypad rows - pin 33, pin 22, pin 21, pin 4, pin 13, pin 12 in order

also my programs that run on the same hardware:
- https://github.com/GreenSlimeStudios/KarpOS_ST7789
- https://github.com/GreenSlimeStudios/ultimate_tic_tac_toe_esp32