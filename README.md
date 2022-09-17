# Advanced Timer 
This project aims to create a digital hourglass to be displayed on a LED-matrix.
The 3D-printed casing for this hourglass is fitted with dials to set the timer and change settings and a buzzer to sound an alarm tone when the timer is finished.
An Arduino Nano controls the simulation and is connected to the input dials as well.

# CAD part
The casing was designed in Autodesk Fusion 360 and consists of two main parts: The display case, featuring a 7-segment display and the LED-matrix, and the control case, housing the Arduino, power supply and input dials.
The different parts of each case and the finished casings themselves are to be 3d-printed and fitted with M3x5,7mm threaded inserts and screwed together using countersunk M3x10 and M3x14 screws.

# Assembly part
The LED-matrix consists of two daisy-chained modules using the SPI bus. These modules should be glued together (hot glue works well) and the SPI bus extended to the second module before assembly of the case starts. 
Data and power cables for the two modules inside the display casing have to be routed into the control casing via a small connector hole in the perimeter of the casings. The power supply, consisting of a 9V battery block, can be connected to the VIN/GND ports of the Arduino via the ON/OFF switch. The Arduino will transform the +9V to +5V for internal use and for external consumers like in our case. Since every breakout board used in this build requires a GND and +5V connection, a dedicated terminal for each of these is recommended. A WAGO 5-port splicing connector is sufficient and the control case has room for two (one for each of the aforementioned potentials). Once the rotary encoder (= input dial) has been connected to the power connectors as well and all data cables have been connected to the specified ports of the Arduino, it can be powered either via internal power source using the ON/OFF switch or using the exposed USB port of the Arduino. The latter will also be used to flash the program.

# Programming 
The firmware for this project was written in C++, using the platformio framework. The Arduino can be flashed without disassembling the case using the exposed USB port like mentioned above.
The behavior of the firmware can be changed in `config.h`.