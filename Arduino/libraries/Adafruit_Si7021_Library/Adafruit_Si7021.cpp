/*!
 * @file Adafruit_Si7021.cpp
 *
 *  @mainpage Adafruit Si7021 breakout board
 *
 *  @section intro_sec Introduction
 *
 *  This is a library for the Si7021 Temperature & Humidity Sensor.
 *
 *  Designed specifically to work with the Adafruit Si7021 Breakout Board.
 *
 *  Pick one up today in the adafruit shop!
 *  ------> https://www.adafruit.com/product/3251
 *
 *  These sensors use I2C to communicate, 2 pins are required to interface.
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit andopen-source hardware by purchasing products
 *  from Adafruit!
 *
 *  @section author Author
 *
 *  Limor Fried (Adafruit Industries)
 *
 *  @section license License
 *
 *  BSD license, all text above must be included in any redistribution
 */

#include "Arduino.h"
#include <Adafruit_Si7021.h>

/*!
 *  @brief  Instantiates a new Adafruit_Si7021 class
 *  @param  *theWire
 *          optional wire object
 */
Adafruit_Si7021::Adafruit_Si7021(TwoWire *theWire) {
  i2c_dev = new Adafruit_I2CDevice(SI7021_DEFAULT_ADDRESS, theWire);

  sernum_a = sernum_b = 0;
  _model = SI_UNKNOWN;
  _revision = 0;
}

/*!
 *  @brief  Sets up the HW by reseting It, reading serial number and reading
 * revision.
 *  @return Returns true if set up is successful.
 */
bool Adafruit_Si7021::begin() {
  if (!i2c_dev->begin())
    return false;

  reset();
  if (_readRegister8(SI7021_READRHT_REG_CMD) != 0x3A)
    return false;

  readSerialNumber();
  _readRevision();

  return true;
}

/*!
 *  @brief  Reads the humidity value from Si7021 (No Master hold)
 *  @return Returns humidity as float value or NAN when there is error timeout
 */
float Adafruit_Si7021::readHumidity() {
  uint8_t buffer[3] = {SI7021_MEASRH_NOHOLD_CMD, 0, 0};

  if (!i2c_dev->write(buffer, 1))
    return NAN;

  delay(20); // account for conversion time for reading humidity

  uint32_t start = millis(); // start timeout
  while (millis() - start < _TRANSACTION_TIMEOUT) {
    if (i2c_dev->read(buffer, 3)) {
      uint16_t hum = buffer[0] << 8 | buffer[1];
      // uint8_t chxsum = buffer[2];

      float humidity = hum;
      humidity *= 125;
      humidity /= 65536;
      humidity -= 6;

      return humidity > 100.0 ? 100.0 : humidity;
    }
    delay(6); // 1/2 typical sample processing time
  }
  return NAN; // Error timeout
}

/*!
 *  @brief  Reads the temperature value from Si7021 (No Master hold)
 *  @return Returns temperature as float value or NAN when there is error
 * timeout
 */
float Adafruit_Si7021::readTemperature() {
  uint8_t buffer[3] = {SI7021_MEASTEMP_NOHOLD_CMD, 0, 0};

  if (!i2c_dev->write(buffer, 1))
    return NAN;

  delay(20); // account for conversion time for reading temperature

  uint32_t start = millis(); // start timeout
  while (millis() - start < _TRANSACTION_TIMEOUT) {
    if (i2c_dev->read(buffer, 3)) {
      uint16_t temp = buffer[0] << 8 | buffer[1];
      // uint8_t chxsum = buffer[2];

      float temperature = temp;
      temperature *= 175.72;
      temperature /= 65536;
      temperature -= 46.85;
      return temperature;
    }
    delay(6); // 1/2 typical sample processing time
  }

  return NAN; // Error timeout
}

/*!
 *  @brief  Sends the reset command to Si7021.
 */
void Adafruit_Si7021::reset() {
  uint8_t buffer[1] = {SI7021_RESET_CMD};
  i2c_dev->write(buffer, 1);

  delay(50);
}

void Adafruit_Si7021::_readRevision(void) {
  uint8_t buffer[2] = {(uint8_t)(SI7021_FIRMVERS_CMD >> 8),
                       (uint8_t)(SI7021_FIRMVERS_CMD & 0xFF)};

  i2c_dev->write_then_read(buffer, 2, buffer, 1);

  switch (buffer[0]) {
  case SI7021_REV_1:
    _revision = 1;
    break;
  case SI7021_REV_2:
    _revision = 2;
    break;
  default:
    _revision = 0;
  }
}

/*!
 *  @brief  Reads serial number and stores It in sernum_a and sernum_b variable
 */
void Adafruit_Si7021::readSerialNumber() {
  uint8_t buffer[8];

  //
  // SNA
  //
  buffer[0] = (uint8_t)(SI7021_ID1_CMD >> 8);
  buffer[1] = (uint8_t)(SI7021_ID1_CMD & 0xFF);
  i2c_dev->write_then_read(buffer, 2, buffer, 8);
  sernum_a = uint32_t(buffer[0]) << 24 | uint32_t(buffer[2]) << 16 |
             uint32_t(buffer[4]) << 8 | uint32_t(buffer[6]);

  //
  // SNB
  //
  buffer[0] = (uint8_t)(SI7021_ID2_CMD >> 8);
  buffer[1] = (uint8_t)(SI7021_ID2_CMD & 0xFF);
  i2c_dev->write_then_read(buffer, 2, buffer, 6);
  sernum_b = uint32_t(buffer[0]) << 24 | uint32_t(buffer[1]) << 16 |
             uint32_t(buffer[4]) << 8 | uint32_t(buffer[5]);

  // Use SNB3 for device model version
  switch (buffer[0]) {
  case 0:
  case 0xff:
    _model = SI_Engineering_Samples;
    break;
  case 0x0D:
    _model = SI_7013;
    break;
  case 0x14:
    _model = SI_7020;
    break;
  case 0x15:
    _model = SI_7021;
    break;
  default:
    _model = SI_UNKNOWN;
  }
}

/*!
 *  @brief  Returns sensor model established during init
 *  @return model value
 */
si_sensorType Adafruit_Si7021::getModel() { return _model; }

/*!
 *  @brief  Enable/Disable sensor heater
 */
void Adafruit_Si7021::heater(bool h) {
  uint8_t regValue = _readRegister8(SI7021_READRHT_REG_CMD);

  if (h) {
    regValue |= (1 << (SI7021_REG_HTRE_BIT));
  } else {
    regValue &= ~(1 << (SI7021_REG_HTRE_BIT));
  }
  _writeRegister8(SI7021_WRITERHT_REG_CMD, regValue);
}

/*!
 *  @brief  Return sensor heater state
 *  @return heater state (TRUE = enabled, FALSE = disabled)
 */
bool Adafruit_Si7021::isHeaterEnabled() {
  uint8_t regValue = _readRegister8(SI7021_READRHT_REG_CMD);
  return (bool)bitRead(regValue, SI7021_REG_HTRE_BIT);
}

/*!
 *  @brief  Set the sensor heater heat level
 */
void Adafruit_Si7021::setHeatLevel(uint8_t level) {
  _writeRegister8(SI7021_WRITEHEATER_REG_CMD, level);
}

/*******************************************************************/

void Adafruit_Si7021::_writeRegister8(uint8_t reg, uint8_t value) {
  uint8_t buffer[2] = {reg, value};
  i2c_dev->write(buffer, 2);

  // Serial.print("Wrote $"); Serial.print(reg, HEX); Serial.print(": 0x");
  // Serial.println(value, HEX);
}

uint8_t Adafruit_Si7021::_readRegister8(uint8_t reg) {
  uint8_t buffer[1] = {reg};
  i2c_dev->write_then_read(buffer, 1, buffer, 1);
  return buffer[0];
}
