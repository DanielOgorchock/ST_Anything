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
 * This library depends on <a href="https://github.com/adafruit/Adafruit_Sensor">
 * Adafruit_Sensor</a> being present on your system. Please make sure you have
 * installed the latest version before using this library.
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
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
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
      @param theI2C Optional pointer to a TwoWire object that should be used for I2C communication. Defaults to &Wire.
      @param tempSensorId the id that should be used for the temperature sensor. Defaults to -1
      @param humiditySensorId the id that should be used for the humidity sensor. Defaults to -1
  */
  /**************************************************************************/
Adafruit_AM2320::Adafruit_AM2320(TwoWire *theI2C, int32_t tempSensorId, int32_t humiditySensorId):
  _temp(this, tempSensorId),
  _humidity(this, humiditySensorId),
  _i2c(theI2C)
{}

/**************************************************************************/
/*! 
    @brief  Setups the hardware
    @return true
*/
/**************************************************************************/
bool Adafruit_AM2320::begin() {
  _i2caddr = 0x5C;  // fixed addr
  _i2c->begin();
  return true;
}

/**************************************************************************/
/*! 
    @brief  read the temperature from the device
    @return the temperature reading as a floating point value
*/
/**************************************************************************/
float Adafruit_AM2320::readTemperature() {
  uint16_t t = readRegister16(AM2320_REG_TEMP_H);
  float ft;
  if (t == 0xFFFF) return NAN;
  // check sign bit - the temperature MSB is signed , bit 0-15 are magnitude
  if(t & 0x8000){
    ft = -(int16_t)(t&0x7fff);
  }
  else {
    ft = (int16_t)t;
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
  uint16_t h = readRegister16(AM2320_REG_HUM_H);
  if (h == 0xFFFF) return NAN;

  float fh = h;
  return fh / 10.0;
}

/**************************************************************************/
/*! 
    @brief  read 2 bytes from a hardware register
    @param reg the register to read
    @return the read value as a 2 byte unsigned integer
*/
/**************************************************************************/
uint16_t Adafruit_AM2320::readRegister16(uint8_t reg) {
  // wake up
  Wire.beginTransmission(_i2caddr);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(10); // wait 10 ms

  // send a command to read register
  Wire.beginTransmission(_i2caddr);
  Wire.write(AM2320_CMD_READREG);
  Wire.write(reg);
  Wire.write(2);  // 2 bytes
  Wire.endTransmission();

  delay(2);  // wait 2 ms
  
  // 2 bytes preamble, 2 bytes data, 2 bytes CRC
  Wire.requestFrom(_i2caddr, (uint8_t)6);
  if (Wire.available() != 6)
    return 0xFFFF;
  
  uint8_t buffer[6];
  for (int i=0; i<6; i++) {
    buffer[i] = Wire.read();
    //Serial.print("byte #"); Serial.print(i); Serial.print(" = 0x"); Serial.println(buffer[i], HEX);
  }

  if (buffer[0] != 0x03)   return 0xFFFF; // must be 0x03 modbus reply
  if (buffer[1] != 2)      return 0xFFFF; // must be 2 bytes reply
  
  uint16_t the_crc = buffer[5];
  the_crc <<= 8;
  the_crc |= buffer[4];
  uint16_t calc_crc = crc16(buffer, 4); // preamble + data
  //Serial.print("CRC: 0x"); Serial.println(calc_crc, HEX);
  if (the_crc != calc_crc)
    return 0xFFFF;

  // All good!
  uint16_t ret = buffer[2];
  ret <<= 8;
  ret |= buffer[3];

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
  for (int i=0; i<nbytes; i++) {
    uint8_t b = buffer[i];
    crc ^= b;
    for (int x=0; x<8; x++) {
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
void Adafruit_AM2320::setName(sensor_t* sensor) {
  strncpy(sensor->name, "AM2320", sizeof(sensor->name) - 1);
}

/**************************************************************************/
/*! 
    @brief  set minimum delay to a fixed value of 2 seconds
    @param sensor a pointer to the sensor
*/
/**************************************************************************/
void Adafruit_AM2320::setMinDelay(sensor_t* sensor) {
  sensor->min_delay = 2000000L;  // 2 seconds (in microseconds)
}

/**************************************************************************/
/*! 
    @brief  create a temperature sensor instance
    @param parent the pointer to the parent sensor
    @param id the id value for the sensor
*/
/**************************************************************************/
Adafruit_AM2320::Temperature::Temperature(Adafruit_AM2320* parent, int32_t id):
  _parent(parent),
  _id(id)
{}

/**************************************************************************/
/*! 
    @brief read the temperature from the device and populate a sensor_event_t with the value
    @param event a pointer to the event to populate
    @return true
*/
/**************************************************************************/
bool Adafruit_AM2320::Temperature::getEvent(sensors_event_t* event) {
  // Clear event definition.
  memset(event, 0, sizeof(sensors_event_t));
  // Populate sensor reading values.
  event->version     = sizeof(sensors_event_t);
  event->sensor_id   = _id;
  event->type        = SENSOR_TYPE_AMBIENT_TEMPERATURE;
  event->timestamp   = millis();
  event->temperature = _parent->readTemperature();
  
  return true;
}

/**************************************************************************/
/*! 
    @brief populate a sensor_t with data for this sensor
    @param sensor a pointer to the sensor_t to populate
*/
/**************************************************************************/
void Adafruit_AM2320::Temperature::getSensor(sensor_t* sensor) {
  // Clear sensor definition.
  memset(sensor, 0, sizeof(sensor_t));
  // Set sensor name.
  _parent->setName(sensor);
  // Set version and ID
  sensor->version         = AM2320_SENSOR_VERSION;
  sensor->sensor_id       = _id;
  // Set type and characteristics.
  sensor->type            = SENSOR_TYPE_AMBIENT_TEMPERATURE;
  _parent->setMinDelay(sensor);

  // This isn't documented, of course
  sensor->max_value   = 80.0F;
  sensor->min_value   = -20.0F;
  sensor->resolution  = 2.0F;
}

/**************************************************************************/
/*! 
    @brief  create a humidity sensor instance
    @param parent the pointer to the parent sensor
    @param id the id value for the sensor
*/
/**************************************************************************/
Adafruit_AM2320::Humidity::Humidity(Adafruit_AM2320* parent, int32_t id):
  _parent(parent),
  _id(id)
{}

/**************************************************************************/
/*! 
    @brief read the humidity from the device and populate a sensor_event_t with the value
    @param event a pointer to the event to populate
    @return true
*/
/**************************************************************************/
bool Adafruit_AM2320::Humidity::getEvent(sensors_event_t* event) {
  // Clear event definition.
  memset(event, 0, sizeof(sensors_event_t));
  // Populate sensor reading values.
  event->version           = sizeof(sensors_event_t);
  event->sensor_id         = _id;
  event->type              = SENSOR_TYPE_RELATIVE_HUMIDITY;
  event->timestamp         = millis();
  event->relative_humidity = _parent->readHumidity();
  
  return true;
}

/**************************************************************************/
/*! 
    @brief populate a sensor_t with data for this sensor
    @param sensor a pointer to the sensor_t to populate
*/
/**************************************************************************/
void Adafruit_AM2320::Humidity::getSensor(sensor_t* sensor) {
  // Clear sensor definition.
  memset(sensor, 0, sizeof(sensor_t));
  // Set sensor name.
  _parent->setName(sensor);
  // Set version and ID
  sensor->version         = AM2320_SENSOR_VERSION;
  sensor->sensor_id       = _id;
  // Set type and characteristics.
  sensor->type            = SENSOR_TYPE_RELATIVE_HUMIDITY;
  _parent->setMinDelay(sensor);

  // This isn't documented, of course
  sensor->max_value   = 100.0F;
  sensor->min_value   = 0.0F;
  sensor->resolution  = 1.0F;
}
