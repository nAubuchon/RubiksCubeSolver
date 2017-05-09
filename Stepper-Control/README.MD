# Stepper-Control

Accepts byte-instructions from python script over usart and controls 6 steppers using
an a4988 stepper-control board with 1/16th-step hard-wired pin configuration.

Notes:
- Target: Custom XMega128A1-based board using USART for dataIO and GPIO breakouts for stepper control signals.
- Linear stepper acceleration timings defined at top of main.c, adjusted for specific cubes used in mechanism.
- Contains code for manual control with push-buttons
- Serial port interfacing recieves byte-instruction, performs intended move, and then sends a '1' back to python script when complete


Bit masks for instructions are as follows:

Face -- Bitmask (dir => x = 0|1) -- Dec. Value
- Up -- 0000 001x -- 2|3
- Right -- 0000 010x -- 4|5
- Front -- 0000 100x -- 8|9
- Down -- 0001 000x -- 16|17
- Left -- 0010 000x -- 32|33
- Back -- 0100 000x -- 64|65
