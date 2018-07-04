/*

  Example of BH1750 library usage.

  This example initialises the BH1750 object using the high resolution
  one-time mode and then makes a light level reading every second.

  The BH1750 component starts up in default mode when it next powers up.
  The BH1750 library automatically reconfigures the one-time mode in
  preparation for the next measurement.

*/

#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

void setup(){

  Serial.begin(9600);

  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();
  // On esp8266 you can select SCL and SDA pins using Wire.begin(D4, D3);

  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);

  Serial.println(F("BH1750 One-Time Test"));

}


void loop() {

  uint16_t lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  delay(1000);

}
