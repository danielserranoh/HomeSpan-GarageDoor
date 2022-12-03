# HomeSpan-GarageDoor

DIY Project to create a Garage Door Controller for HomeKit based on ESP32 and [HomeSpan](https://github.com/HomeSpan/HomeSpan)

The garage door target to be controlled is the BFT DEIMOS ULTRA - with a MERAK controller board, although the logic should work with a HAMAL Board as well.

The main goal of the system is to habilitate the interaction with Homekit. The system is going to wrap the MERAK Controller Board. The Merak board will still be resposible of the control of the motor, as well as the photo sensor and the lightbulb. 
However, I/O of the system will interact with the board enhancing some features. In addition it will interact with the pedestrian door (portero automatico). 


The repository has three branches:
- *prototype* Use for HW/SW development on the breadboard -> Actuators are LEDS
- *development* Use for SW development on the physical prototype -> Actuators are relays and LEDS
- *main* Use for the release of the firmware


## SYSTEM DESCRIPTION
The system consists of:

*2 Hall effect digital sensors*

These two sensors are set apart several cm and are activated by magnets attached to the extremes of the door rack.
As a result one sensor can detect if the door is closed, the other sensor can detect is the door is open.
Combining the signal of both we can infer if the door is moving (opening or closing)

*1 Reed Sensor*

This sensor detects the open/close state of a pedestrian door that intersects the path of the garage door. If the Reed senror is open, the garage door must stop 

**1 Photovoltaic Sensor**

This sensor detects if there is an obstacle in the path of the garage door.

*1 RELAY MODULE*

The relay module has 3 relays controling the garage door and the pedestrian door. The signals required are:
1. Pulse for triggering the door movement: OPEN/STOP/CLOSE cycle
2. Switch for locking the Garage door mechanism => Avoids the opening of the door regardless of the order. This Switch is triggered by the reed sensor and a Homekit virtual switch
3. Pulse to open the pedestrian door.



## Roadmap
SW Memorise last states (just in case there is a power off)
SW 

HW voltage adjuster for input signals 
  - ReedSensor Vcc => 3,3V
  - Photosensor Vac 24V => 3,3V


HW Test 4 x Solid State Relay Module: will add a relay for pedestrian opening of the garage door