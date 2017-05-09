# Imaging

Image processing script for Solver.  Work in Progress.
- image_proc.py is the working interface for solving and sending data to steppers from pre-captured images
- realtime.py is testing the object tracking with cameras and gathering state-data of any configuration of cube
- object_adjust.py is for testing image kernals with object tracking to lower necessary accuracy for camera positioning
- ignore other files

Interfaces with cameras to determine cube state, and passes information to Kociemba cube solving software to obtain physical 
steps necessary to solve the cube.  Then parses instructions into bytecode to be sent to microcontroller through USART.

Notes:
- Requires numpy/matplotlib, cv2, Kociemba, and pyserial libraries
- Parsing of byte-instructions specified in Stepper-Control folder README
