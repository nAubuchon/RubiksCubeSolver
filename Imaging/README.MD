# Imaging

Image processing script for Solver

Interfaces with cameras to determine cube state, and passes information to Kociemba cube solving software to obtain physical 
steps necessary to solve the cube.  Then parses instructions into bytecode to be sent to microcontroller through USART.

Notes:
- Requires numpy/matplotlib, cv2, Kociemba, and pyserial libraries
- Parsing of byte-instructions specified in Stepper-Control folder README
