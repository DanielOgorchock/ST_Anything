/*!
 * @file Adafruit_MCP23008.cpp
 *
 * @mainpage Adafruit MCP23008 Library
 *
 * @section intro_sec Introduction
 *
 * This is a library for the MCP23008 i2c port expander
 *
 * These displays use I2C to communicate, 2 pins are required to
 * interface
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section author Author
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text above must be included in any redistribution
 */

#include "Adafruit_MCP23008.h"

///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Begins the i2c connection using specified address
 * @param addr i2c address of the MCP23008, defaults to 0x20
 * @param wire TwoWire interface, defaults to &Wire
 * @returns False if failed to find device at address
 */
bool Adafruit_MCP23008::begin(uint8_t addr, TwoWire *wire) {
  if ((addr >= 0x20) && (addr <= 0x27)) {
    _i2caddr = addr;
  } else if (addr <= 0x07) {
    _i2caddr = 0x20 + addr;
  } else {
    _i2caddr = 0x27;
  }

  if (i2c_dev) {
    delete i2c_dev; // remove old interface
  }

  i2c_dev = new Adafruit_I2CDevice(_i2caddr, wire);

  if (!i2c_dev->begin()) {
    return false;
  }

  // set defaults!
  uint8_t buf[] = {MCP23008_IODIR,
                   0xFF, // all inputs
                   0x00,           0x00, 0x00, 0x00, 0x00,
                   0x00,           0x00, 0x00, 0x00};
  if (!i2c_dev->write(buf, 10)) {
    return false;
  }

  return true;
}

/*!
 * @brief Sets the pin mode
 * @param p Mode to set
 * @param d Pin to set the mode to
 * @returns True on success, False on bad input or I2C failure
 */
bool Adafruit_MCP23008::pinMode(uint8_t p, uint8_t d) {
  uint8_t iodir;

  // only 8 bits!
  if (p > 7)
    return false;

  iodir = read8(MCP23008_IODIR);

  // set the pin and direction
  if (d == INPUT) {
    iodir |= 1 << p;
  } else {
    iodir &= ~(1 << p);
  }

  // write the new IODIR
  return write8(MCP23008_IODIR, iodir);
}

/*!
 * @brief Reads the current GPIO input
 * @return Returns the current GPIO input
 */
uint8_t Adafruit_MCP23008::readGPIO(void) {
  // read the current GPIO input
  return read8(MCP23008_GPIO);
}

/*!
 * @brief Writes to the GPIO
 * @param gpio what to write
 * @returns True on success, False on bad input or I2C failure
 */
bool Adafruit_MCP23008::writeGPIO(uint8_t gpio) {
  return write8(MCP23008_GPIO, gpio);
}

/*!
 * @brief Sets the pin and direction
 * @param p Pin to set
 * @param d Direction to set the pin
 * @returns True on success, False on bad input or I2C failure
 */
bool Adafruit_MCP23008::digitalWrite(uint8_t p, uint8_t d) {
  uint8_t gpio;

  // only 8 bits!
  if (p > 7)
    return false;

  // read the current GPIO output latches
  gpio = readGPIO();

  // set the pin and direction
  if (d == HIGH) {
    gpio |= 1 << p;
  } else {
    gpio &= ~(1 << p);
  }

  // write the new GPIO
  return writeGPIO(gpio);
}

/*!
 * @brief Sets pull-up resistor on specified pin
 * @param p Pin to set
 * @param d Direction to set the pin
 * @returns True on success, False on bad input or I2C failure
 */
bool Adafruit_MCP23008::pullUp(uint8_t p, uint8_t d) {
  uint8_t gppu;

  // only 8 bits!
  if (p > 7)
    return false;

  gppu = read8(MCP23008_GPPU);
  // set the pin and direction
  if (d == HIGH) {
    gppu |= 1 << p;
  } else {
    gppu &= ~(1 << p);
  }
  // write the new GPIO
  return write8(MCP23008_GPPU, gppu);
}

/*!
 * @brief Reads the status of a gpio pin
 * @param p Pin to read
 * @return Returns the current gpio
 */
uint8_t Adafruit_MCP23008::digitalRead(uint8_t p) {
  // only 8 bits!
  if (p > 7)
    return 0;

  // read the current GPIO
  return (readGPIO() >> p) & 0x1;
}

uint8_t Adafruit_MCP23008::read8(uint8_t addr) {
  Adafruit_BusIO_Register reg = Adafruit_BusIO_Register(i2c_dev, addr, 1);
  return reg.read();
}

bool Adafruit_MCP23008::write8(uint8_t addr, uint8_t data) {
  Adafruit_BusIO_Register reg = Adafruit_BusIO_Register(i2c_dev, addr, 1);

  return reg.write(data);
}
