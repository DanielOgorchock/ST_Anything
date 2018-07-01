/*!
 * @file Adafruit_AM2320.h
 *
 * This is a library for the AM2320 Temperature & Humidity Unified Sensor breakout board
 * ----> https://www.adafruit.com/products/xxxx
 * 
 * Adafruit invests time and resources providing this open source code, 
 * please support Adafruit and open-source hardware by purchasing 
 * products from Adafruit!
 *
 * Written by Limor Fried for Adafruit Industries.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */


#ifndef ADAFRUIT_AM2320
#define ADAFRUIT_AM2320

#define AM2320_SENSOR_VERSION 1 ///< the sensor version
#define AM2320_CMD_READREG    0x03 ///< read register command
#define AM2320_REG_TEMP_H     0x02 ///< temp register address
#define AM2320_REG_HUM_H      0x00 ///< humidity register address
#include <Adafruit_Sensor.h>
#include <Wire.h>

/**************************************************************************/
/*! 
    @brief  Class that stores state and functions for interacting with AM2320 Temperature & Humidity Unified Sensor
*/
/**************************************************************************/
class Adafruit_AM2320 {
public:
  Adafruit_AM2320(TwoWire *theI2C = &Wire, int32_t tempSensorId=-1, int32_t humiditySensorId=-1);
  bool begin();

  float readTemperature();
  float readHumidity();
  uint16_t readRegister16(uint8_t reg);
  uint16_t crc16(uint8_t *buffer, uint8_t nbytes);

  /**************************************************************************/
  /*! 
      @brief  temperature sensor class
  */
  /**************************************************************************/
  class Temperature : public Adafruit_Sensor {
  public:
    Temperature(Adafruit_AM2320* parent, int32_t id);
    bool getEvent(sensors_event_t* event);
    void getSensor(sensor_t* sensor);

  private:
    Adafruit_AM2320* _parent; ///< the parent sensor object
    int32_t _id; ///< the sensor id
  };

  /**************************************************************************/
  /*! 
      @brief  humidity sensor class
  */
  /**************************************************************************/
  class Humidity : public Adafruit_Sensor {
  public:
    Humidity(Adafruit_AM2320* parent, int32_t id);
    bool getEvent(sensors_event_t* event);
    void getSensor(sensor_t* sensor);

  private:
     Adafruit_AM2320* _parent; ///< the parent sensor object
    int32_t _id; ///< the sensor id

  };

  /**************************************************************************/
  /*! 
      @brief  get the temperature sensor object belonging to this class
      @return the temperature sensor object
  */
  /**************************************************************************/
  Temperature temperature() {
    return _temp;
  }

  /**************************************************************************/
  /*! 
      @brief  get the humidity sensor object belonging to this class
      @return the humidity sensor object
  */
  /**************************************************************************/
  Humidity humidity() {
    return _humidity;
  }

private:
  Temperature _temp; ///< the temperature sensor object
  Humidity _humidity; ///< the humidity sensor object
  TwoWire *_i2c; ///< the TwoWire object used for I2C connumication
  uint8_t _i2caddr; ///< the i2c address the device can be found on

  void setName(sensor_t* sensor);
  void setMinDelay(sensor_t* sensor);
};

#endif
