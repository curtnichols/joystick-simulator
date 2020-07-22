# joystick-simulator

Simulate a joystick over USB.

## Context

I got it in my head to build a joystick simulator for an ATMEGA32U4 based
Arduino-based board, accepting direction from input on the serial port.
Initial steps include:

- [x] Rudimentary receipt, parsing, and response to serial inputs from a user.
- [ ] Achieve working configuration of a new HID device that looks like a joystick while maintaining working serial I/O.
- [ ] Add parsing and feedback for joystick-related control from serial input.

## Platform

This is an Arduino-based project, as it's easy enough to mimic USB HID devices
when using an ATMEGA32U4-based board. Certainly other platforms make this possible,
but that's currently not of interest.

## Miscellaneous

Initial bring-up of the project was done on

- SparkFun Arduino Pro Micro qwiic
- Acrobotic Beetle
