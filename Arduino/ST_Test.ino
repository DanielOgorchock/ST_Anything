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
#include <EX_Switch.h>

#define PIN_ILLUMINANCE          A5
#define PIN_TEMPERATUREHUMIDITY  5
#define PIN_WATER                A4
#define PIN_MOTION               4
#define PIN_SWITCH               6

long lastmillis=0;

//Declare each sensor
st::PS_Illuminance sensor1("illuminance", 60000, 1000, PIN_ILLUMINANCE);
st::PS_TemperatureHumidity sensor2("temphumid", 60000, 2000, PIN_TEMPERATUREHUMIDITY, st::PS_TemperatureHumidity::DHT22);
st::PS_Water sensor3("water", 60000, 3000, PIN_WATER);
st::IS_Motion sensor4("motion", PIN_MOTION, HIGH);
st::EX_Switch executor1("switch", PIN_SWITCH, LOW);

//test sketch
void setup()
{
  st::Everything::debug=true;
  st::Device::debug=true;
  st::PollingSensor::debug=true;
  st::InterruptSensor::debug=true;
 
  st::Everything::init();
  
  //Add each sensor to the "Everything Class"
  st::Everything::addSensor(&sensor1);
  st::Everything::addSensor(&sensor2);
  st::Everything::addSensor(&sensor3);
  st::Everything::addSensor(&sensor4); 
  
  //Add each executor to the "Everything Class"
  st::Everything::addExecutor(&executor1);

  st::Everything::initDevices();
}

void loop()
{
  st::Everything::run();
}
