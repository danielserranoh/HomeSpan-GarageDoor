# HomeSpan-GarageDoor

DIY Project to create a Garage Door Controller for HomeKit based on ESP32 and [HomeSpan](https://github.com/HomeSpan/HomeSpan)

The garage door target to be controlled is the BFT DEIMOS ULTRA - with a MERAK controller board, although it should work with a HAMAL Board as well.

The repository has three branches:
- *prototype* Use for HW/SW development in the breadboard -> Actuators are LEDS
- *development* Use for SW development in the physical prototype -> Actuators are relays and LEDS
- *main* Use for the release of the firmware




Roadmap:
- Add magnetic sensors for the door limits
- Physically integrate the ~24V sensor