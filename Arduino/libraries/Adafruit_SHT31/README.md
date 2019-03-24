# Adafruit SHT31-D Temperature and Humidity Sensor Breakout [![Build Status](https://travis-ci.org/adafruit/Adafruit_SHT31.svg?branch=master)](https://travis-ci.org/adafruit/Adafruit_SHT31)

![Adafruit SHT31](https://cdn-learn.adafruit.com/assets/assets/000/028/970/small360/adafruit_products_2857_iso_ORIG.jpg?1449606376)

This is a library for the SHT31 Digital Humidity + Temp sensor.

It is designed specifically to work with the SHT31 Digital in the Adafruit shop:

- https://www.adafruit.com/products/2857

These sensors use **I2C** to communicate, 2 pins are required to interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.  
BSD license, all text above must be included in any redistribution

Check out the links above for our tutorials and wiring diagrams

## Installation

Use the Arduino Library Manager to install this library. If you're unfamiliar
with how this works, we have a great tutorial on Arduino library installation
at: http://learn.adafruit.com/adafruit-all-about-arduino-libraries-install-use

<!-- START COMPATIBILITY TABLE -->

## Compatibility

MCU                | Tested Works | Doesn't Work | Not Tested  | Notes
------------------ | :----------: | :----------: | :---------: | -----
Atmega328 @ 16MHz  |      X       |             |            |
Atmega328 @ 12MHz  |      X       |             |            |
Atmega32u4 @ 16MHz |      X       |             |            |
Atmega32u4 @ 8MHz  |      X       |             |            |
ESP8266            |      X       |             |            |
Atmega2560 @ 16MHz |      X       |             |            |
ATSAM3X8E          |      X       |             |            |
ATSAM21D           |      X       |             |            |
ATtiny85 @ 16MHz   |      X       |             |            |
ATtiny85 @ 8MHz    |      X       |             |            |
Intel Curie @ 32MHz |             |             |     X       |
STM32F2            |             |             |     X       |

  * ATmega328 @ 16MHz : Arduino UNO, Adafruit Pro Trinket 5V, Adafruit Metro 328, Adafruit Metro Mini
  * ATmega328 @ 12MHz : Adafruit Pro Trinket 3V
  * ATmega32u4 @ 16MHz : Arduino Leonardo, Arduino Micro, Arduino Yun, Teensy 2.0
  * ATmega32u4 @ 8MHz : Adafruit Flora, Bluefruit Micro
  * ESP8266 : Adafruit Huzzah
  * ATmega2560 @ 16MHz : Arduino Mega
  * ATSAM3X8E : Arduino Due
  * ATSAM21D : Arduino Zero, M0 Pro
  * ATtiny85 @ 16MHz : Adafruit Trinket 5V
  * ATtiny85 @ 8MHz : Adafruit Gemma, Arduino Gemma, Adafruit Trinket 3V

<!-- END COMPATIBILITY TABLE -->
