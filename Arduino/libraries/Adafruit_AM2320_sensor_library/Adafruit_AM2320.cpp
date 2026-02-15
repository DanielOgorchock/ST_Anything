/*!
 * @file Adafruit_AM2320.cpp
 *
 * @mainpage Adafruit AM2320 Temperature & Humidity Unified Sensor driver
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's AM2320 driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit AM2320 breakout: https://www.adafruit.com/products/xxxx
 *
 * These sensors use I2C to communicate, 2 pins (SCL+SDA) are required
 * to interface with the breakout.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section dependencies Dependencies
 *
 * This library depends on <a
 * href="https://github.com/adafruit/Adafruit_Sensor"> Adafruit_Sensor</a> being
 * present on your system. Please make sure you have installed the latest
 * version before using this library.
 *
 * @section author Author
 *
 * Written by Limor Fried for Adafruit Industries.
 *
 * @section license License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "Adafruit_AM2320.h"

/**************************************************************************/
/*!
    @brief  Instantiates a new AM2320 class
    @param theI2C Optional pointer to a TwoWire object that should be used for
   I2C communication. Defaults to &Wire.
    @param tempSensorId the id that should be used for the temperature sensor.
   Defaults to -1
    @param humiditySensorId the id that should be used for the humidity sensor.
   Defaults to -1
*/
/**************************************************************************/
Adafruit_AM2320::Adafruit_AM2320(TwoWire *theI2C, int32_t tempSensorId,
                                 int32_t humiditySensorId)
    : _temp(this, tempSensorId), _humidity(this, humiditySensorId) {
  if (i2c_dev)
    delete i2c_dev;
  i2c_dev = new Adafruit_I2CDevice(0x5C, theI2C); // fixed addr
}

/**************************************************************************/
/*!
    @brief  Setups the hardware
    @return true
*/
/**************************************************************************/
bool Adafruit_AM2320::begin() {
  if (!i2c_dev->begin(false))
    return false;
  // sensor presence check
  i2c_dev->detected();        // wake up
  delay(10);                  // wait 10 ms
  return i2c_dev->detected(); // check if ACK
}

/**************************************************************************/
/*!
    @brief  read the temperature from the device
    @return the temperature reading as a floating point value
*/
/**************************************************************************/
float Adafruit_AM2320::readTemperature() {
  uint32_t data = readRegister32(AM2320_REG_HUM_H);
  if (data == 0xFFFFFFFF)
    return NAN;
  float ft;
  // check sign bit - the temperature MSB is signed , bit 0-15 are magnitude
  if (data & 0x8000) {
    ft = -(int16_t)(data & 0x7FFF);
  } else {
    ft = (int16_t)(data & 0xFFFF);
  }
  return ft / 10.0;
}

/**************************************************************************/
/*!
    @brief  read the humidity from the device
    @return the humidity reading as a floating point value
*/
/**************************************************************************/
float Adafruit_AM2320::readHumidity() {
  uint32_t data = readRegister32(AM2320_REG_HUM_H);
  if (data == 0xFFFFFFFF)
    return NAN;

  return (data >> 16) / 10.0;
}

/**************************************************************************/
/*!
    @brief  read 4 bytes from a hardware register
    @param reg the register to read
    @return the read value as a 4 byte unsigned integer
*/
/**************************************************************************/
uint32_t Adafruit_AM2320::readRegister32(uint8_t reg) {
  uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  bool written = false;

  // wake up
  for (int i = 0; i < 3; i++) {
    written = i2c_dev->write(buffer, 1);
    if (written)
      break;
    delay(100); // 50-70 lead to 2 errors each time
  }
  if (!written)
    return 0xFFFFFFFF; // no ACK!
  delay(10);           // wait 10 ms

  // send a command to read register
  buffer[0] = AM2320_CMD_READREG;
  buffer[1] = reg;
  buffer[2] = 4; // 4 bytes
  for (int i = 0; i < 3; i++) {
    written = i2c_dev->write(buffer, 3);
    if (written)
      break;
    delay(5);
  }
  if (!written)
    return 0xFFFFFFFF; // read not acknowledged!
  delay(2);            // wait 2 ms

  // 2 bytes preamble, 4 bytes data, 2 bytes CRC
  i2c_dev->read(buffer, 8);

  if (buffer[0] != 0x03)
    return 0xFFFFFFFF; // must be 0x03 modbus reply
  if (buffer[1] != 4)
    return 0xFFFFFFFF; // must be 4 bytes reply

  uint16_t the_crc = buffer[7];
  the_crc <<= 8;
  the_crc |= buffer[6];
  uint16_t calc_crc = crc16(buffer, 6); // preamble + data
  // Serial.print("CRC: 0x"); Serial.println(calc_crc, HEX);
  if (the_crc != calc_crc)
    return 0xFFFFFFFF;

  // All good!
  uint32_t ret = uint32_t(buffer[2]) << 24 | uint32_t(buffer[3]) << 16 |
                 uint32_t(buffer[4]) << 8 | uint32_t(buffer[5]);

  return ret;
}

/**************************************************************************/
/*!
    @brief  perfor a CRC check to verify data
    @param buffer the pointer to the data to check
    @param nbytes the number of bytes to calculate the CRC over
    @return the calculated CRC
*/
/**************************************************************************/
uint16_t Adafruit_AM2320::crc16(uint8_t *buffer, uint8_t nbytes) {
  uint16_t crc = 0xffff;
  for (int i = 0; i < nbytes; i++) {
    uint8_t b = buffer[i];
    crc ^= b;
    for (int x = 0; x < 8; x++) {
      if (crc & 0x0001) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

/**************************************************************************/
/*!
    @brief  set the name of the sensor
    @param sensor a pointer to the sensor
*/
/**************************************************************************/
void Adafruit_AM2320::setName(sensor_t *sensor) {
  strncpy(sensor->name, "AM2320", sizeof(sensor->name) - 1);
}

/**************************************************************************/
/*!
    @brief  set minimum delay to a fixed value of 2 seconds
    @param sensor a pointer to the sensor
*/
/**************************************************************************/
void Adafruit_AM2320::setMinDelay(sensor_t *sensor) {
  sensor->min_delay = 2000000L; // 2 seconds (in microseconds)
}

/**************************************************************************/
/*!
    @brief  create a temperature sensor instance
    @param parent the pointer to the parent sensor
    @param id the id value for the sensor
*/
/**************************************************************************/
Adafruit_AM2320::Temperature::Temperature(Adafruit_AM2320 *parent, int32_t id)
    : _parent(parent), _id(id) {}

/**************************************************************************/
/*!
    @brief read the temperature from the device and populate a sensor_event_t
   with the value
    @param event a pointer to the event to populate
    @return true
*/
/**************************************************************************/
bool Adafruit_AM2320::Temperature::getEvent(sensors_event_t *event) {
  // Clear event definition.
  memset(event, 0, sizeof(sensors_event_t));
  // Populate sensor reading values.
  event->version = sizeof(sensors_event_t);
  event->sensor_id = _id;
  event->type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
  event->timestamp = millis();
  event->temperature = _parent->readTemperature();

  return true;
}

/**************************************************************************/
/*!
    @brief populate a sensor_t with data for this sensor
    @param sensor a pointer to the sensor_t to populate
*/
/**************************************************************************/
void Adafruit_AM2320::Temperature::getSensor(sensor_t *sensor) {
  // Clear sensor definition.
  memset(sensor, 0, sizeof(sensor_t));
  // Set sensor name.
  _parent->setName(sensor);
  // Set version and ID
  sensor->version = AM2320_SENSOR_VERSION;
  sensor->sensor_id = _id;
  // Set type and characteristics.
  sensor->type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
  _parent->setMinDelay(sensor);

  // This isn't documented, of course
  sensor->max_value = 80.0F;
  sensor->min_value = -20.0F;
  sensor->resolution = 2.0F;
}

/**************************************************************************/
/*!
    @brief  create a humidity sensor instance
    @param parent the pointer to the parent sensor
    @param id the id value for the sensor
*/
/**************************************************************************/
Adafruit_AM2320::Humidity::Humidity(Adafruit_AM2320 *parent, int32_t id)
    : _parent(parent), _id(id) {}

/**************************************************************************/
/*!
    @brief read the humidity from the device and populate a sensor_event_t with
   the value
    @param event a pointer to the event to populate
    @return true
*/
/**************************************************************************/
bool Adafruit_AM2320::Humidity::getEvent(sensors_event_t *event) {
  // Clear event definition.
  memset(event, 0, sizeof(sensors_event_t));
  // Populate sensor reading values.
  event->version = sizeof(sensors_event_t);
  event->sensor_id = _id;
  event->type = SENSOR_TYPE_RELATIVE_HUMIDITY;
  event->timestamp = millis();
  event->relative_humidity = _parent->readHumidity();

  return true;
}

/**************************************************************************/
/*!
    @brief populate a sensor_t with data for this sensor
    @param sensor a pointer to the sensor_t to populate
*/
/**************************************************************************/
void Adafruit_AM2320::Humidity::getSensor(sensor_t *sensor) {
  // Clear sensor definition.
  memset(sensor, 0, sizeof(sensor_t));
  // Set sensor name.
  _parent->setName(sensor);
  // Set version and ID
  sensor->version = AM2320_SENSOR_VERSION;
  sensor->sensor_id = _id;
  // Set type and characteristics.
  sensor->type = SENSOR_TYPE_RELATIVE_HUMIDITY;
  _parent->setMinDelay(sensor);

  // This isn't documented, of course
  sensor->max_value = 100.0F;
  sensor->min_value = 0.0F;
  sensor->resolution = 1.0F;
}
