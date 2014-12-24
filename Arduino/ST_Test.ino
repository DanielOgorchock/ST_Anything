#include <Device.h>
#include <InterruptSensor.h>
#include <PollingSensor.h>
#include <PS_Illuminance.h>
#include <Sensor.h>

const unsigned int SENSOR_COUNT=11;
st::Sensor *sensors[SENSOR_COUNT];

st::PollingSensor test0("Temperature", 10000);
st::PollingSensor test1("Humidity", 20000);
st::PollingSensor test2("GarageDoor1", 8000);
st::PollingSensor test3("GarageDoor2", 8000, 4000);
st::PollingSensor test4("LightSensor", 30000);
st::InterruptSensor test5("GarageButton", 8, HIGH);
st::InterruptSensor test6("Doorbell", 9, HIGH);
st::PS_Illuminance test7("illuminance", 5000, 0, A5);

//test sketch
void setup()
{
  st::Device::DEBUG=true;
  st::PollingSensor::DEBUG=true;
  st::InterruptSensor::DEBUG=true;
  Serial.begin(9600);
  
  sensors[0]=(st::Sensor*)&test0; //new st::PollingSensor("Temperature", 10000);
  sensors[1]=(st::Sensor*)&test1; //new st::PollingSensor("Humidity", 20000);
  sensors[2]=(st::Sensor*)&test2; //new st::PollingSensor("GarageDoor1", 4000);
  sensors[3]=(st::Sensor*)&test3; //new st::PollingSensor("GarageDoor2", 4000, 2000);
  sensors[4]=(st::Sensor*)&test4; //new st::PollingSensor("LightSensor", 30000);
  sensors[5]=(st::Sensor*)&test5; //new st::InterruptSensor("GarageButton", 8, HIGH);
  sensors[6]=(st::Sensor*)&test6; //new st::InterruptSensor("Doorbell", 9, HIGH);
  sensors[7]=(st::Sensor*)&test7; //new st::InterruptSensor("Alarm", 10, HIGH);
  sensors[8]=new st::InterruptSensor("Alert", 11, HIGH);
  sensors[9]=new st::InterruptSensor("GarbageDisposal", 12, HIGH);
  sensors[10]=new st::PollingSensor("dynamicPolling", 2000, 0);
  }

void loop()
{
  for(unsigned int count=0; count<SENSOR_COUNT; ++count)
  {
     sensors[count]->update(); 
  }
}
