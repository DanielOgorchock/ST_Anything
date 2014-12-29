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
#include <IS_Motion.h>
#include <Sensor.h>

//Declare each sensor
st::PS_Illuminance sensor1("illuminance", 30000, -30000, A5);
//st::PS_TemperatureHumidity sensor2("temphumid", 30000, -20000, 5);
st::PS_Water sensor3("water", 30000, -10000, A4);
//st::IS_Motion sensor4("motion", 4, HIGH);

//test sketch
void setup()
{
  st::Everything::debug=true;
  st::Device::debug=true;
  st::PollingSensor::debug=true;
  st::InterruptSensor::debug=true;
  
  st::Everything::init(); //serial initialized internally
  
  //Add each sensor to the "Everything Class"
  st::Everything::addSensor(&sensor1);
  //st::Everything::addSensor(&sensor2);
  st::Everything::addSensor(&sensor3);
  //st::Everything::addSensor(&sensor4); 

  st::Everything::initDevices();
}

void loop()
{
  st::Everything::run();
}
