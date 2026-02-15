# Adafruit MCP23008 Library [![Build Status](https://github.com/adafruit/Adafruit-MCP23008-library/workflows/Arduino%20Library%20CI/badge.svg)](https://github.com/adafruit/Adafruit-MCP23008-library/actions)[![Documentation](https://github.com/adafruit/ci-arduino/blob/master/assets/doxygen_badge.svg)](http://adafruit.github.io/Adafruit-MCP23008-library/html/index.html)

To download, click "Download Source" in the top right corner. Then install as indicated in our tutorial:
--> http://www.ladyada.net/library/arduino/libraries.html
in a folder called Adafruit_MCP23008.

You will also need the Adafruit BusIO library: https://github.com/adafruit/Adafruit_BusIO

This is very much beta, it seems to work fine but its not optimized and doesn't currently suport the interrupt capability of the chip

Currently it can do: setting pin directions, inputs and outputs and turning on/off the pull-up resistors

To use:
Connect pin #1 of the expander to Analog 5 (i2c clock)
Connect pin #2 of the expander to Analog 4 (i2c data)
Connect pins #3, 4 and 5 of the expander to ground (address selection)
Connect pin #6 and 18 of the expander to 5V (power and reset disable)
Connect pin #9 of the expander to ground (common ground)
Pins 10 thru 17 are your input/output pins

Check the datasheet for more info: http://ww1.microchip.com/downloads/en/DeviceDoc/21919b.pdf

Enjoy and send pull requests!
