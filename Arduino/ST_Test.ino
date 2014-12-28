#include <SoftwareSerial.h>
#include <SmartThings.h>
#include <Constants.h>
#include <Device.h>
#include <Everything.h>
#include <Executor.h>
#include <InterruptSensor.h>
#include <PollingSensor.h>
#include <PS_Illuminance.h>
#include <dht.h>
#include <PS_TemperatureHumidity.h>
#include <PS_Water.h>
#include <Sensor.h>

int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

long lastmillis=0;

//Declare each sensor
st::PS_Illuminance sensor1("illuminance", 30000, -30000, A5);
st::PS_TemperatureHumidity sensor2("TempHumid", 30000, -20000, 5);
st::PS_Water sensor3("water", 30000, -10000, A4);

//st::InterruptSensor test5("bogus_name__", 8, HIGH);

//test sketch
void setup()
{
  Serial.begin(9600); //THIS MUST BE CALLED FIRST TO AVOID TERRIBLE RESULTS
  st::Everything::debug=true;
  st::Device::debug=true;
  st::PollingSensor::debug=true;
  st::InterruptSensor::debug=true;
  st::Everything::Return_String.reserve(st::Constants::RETURN_STRING_RESERVE);
  
  Serial.print(F("Setup Init: Free Ram = "));
  Serial.println(freeRam());
  st::Everything::initSmartThings();

  Serial.print(F("Setup Adding Sensors: Free Ram = "));  
  Serial.println(freeRam());
  
  //Add each sensor to the "Everything Class"
  st::Everything::addSensor(&sensor1);
  st::Everything::addSensor(&sensor2);
  st::Everything::addSensor(&sensor3);
  
  Serial.print(F("Setup Added Sensors: Free Ram = "));  
  Serial.println(freeRam());
  st::Everything::initDevices();
  Serial.print(F("Setup Initialized Sensors: Free Ram = "));  
  Serial.println(freeRam());
}

void loop()
{
  st::Everything::run();
  if(millis()%5000==0 && millis()!=lastmillis) {
    lastmillis = millis();
    Serial.print(F("Loop: Free Ram = "));  
    Serial.println(freeRam());
  }
}
