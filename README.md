# DFpong gblekkenhorst homeostatis I
 A weird and wonderful pong controller for Digital Futures Creation and Computation 2024.

![image of the final controller](https://github.com/ameinias/DFpong-gblekkenhorst-homeostatis-brain/blob/main/screenshot%20of%20window%20video.png)

 This program implements a Bluetooth Low Energy controller for Pong.
 It sends movement data to a central device running in the browser and
 provides audio feedback through a buzzer. It uses a pressure sensor to 
 collect data - it takes the starting pressure and calculates threshold 
 above and below that value to decide how much pressure is required to 
 switch between Up/Down/Still. 

Intended to work with this circuitry:
![circuits](https://github.com/ameinias/DFpong-gblekkenhorst-homeostatis-brain/blob/main/hemostasis-circuitdiagram_bb.png)
