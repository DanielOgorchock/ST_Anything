#include <SoftwareSerial.h>  //Arduino UNO/Leonardo uses SoftwareSerial for the SmartThings Librarysa
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
#include <PS_Alarm.h>
#include <IS_Motion.h>
#include <Sensor.h>
#include <EX_Switch.h>

#define PIN_ILLUMINANCE          A5
#define PIN_TEMPERATUREHUMIDITY  5
#define PIN_WATER                A4
#define PIN_MOTION               4
#define PIN_SWITCH               8
#define PIN_ALARM                9

//Declare each sensor

//Polling Sensors
st::PS_Illuminance sensor1("illuminance", 15000, 0, PIN_ILLUMINANCE);
st::PS_TemperatureHumidity sensor2("temphumid", 15000, 3000, PIN_TEMPERATUREHUMIDITY, st::PS_TemperatureHumidity::DHT22);
st::PS_Water sensor3("water", 15000, 6000, PIN_WATER);
st::PS_Alarm sensor4("alarm", 15000, 9000, PIN_ALARM, LOW, true);

//Interrupt Sensors 
//st::IS_Motion sensor5("motion", PIN_MOTION, HIGH);

//Executors
st::EX_Switch executor1("switch", PIN_SWITCH, LOW, true);

//test sketch
void setup()
{
  st::Everything::debug=true;
  st::Executor::debug=true;
  st::Device::debug=true;
  st::PollingSensor::debug=true;
  st::InterruptSensor::debug=true;
 
//  st::Everything::debug=false;
//  st::Executor::debug=false;
//  st::Device::debug=false;
//  st::PollingSensor::debug=false;
//  st::InterruptSensor::debug=false;
  
  st::Everything::init();
  
  //Add each sensor to the "Everything Class"
  st::Everything::addSensor(&sensor1);
  st::Everything::addSensor(&sensor2);
  st::Everything::addSensor(&sensor3);
  st::Everything::addSensor(&sensor4); 
  //st::Everything::addSensor(&sensor5); 
  
  //Add each executor to the "Everything Class"
  st::Everything::addExecutor(&executor1);

  st::Everything::initDevices();
}

void loop()
{
  st::Everything::run();
}
