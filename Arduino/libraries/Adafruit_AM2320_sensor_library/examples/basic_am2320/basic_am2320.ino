#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"

Adafruit_AM2320 am2320 = Adafruit_AM2320();

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10); // hang out until serial port opens
  }

  Serial.println("Adafruit AM2320 Basic Test");
  am2320.begin();
}

void loop() {
  Serial.print("Temp: "); Serial.println(am2320.readTemperature());
  Serial.print("Hum: "); Serial.println(am2320.readHumidity());

  delay(2000);
}