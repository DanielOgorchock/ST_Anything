#include <Device.h>
#include <InterruptSensor.h>
#include <PollingSensor.h>
#include <PS_Illuminance.h>
#include <Sensor.h>

const unsigned int SENSOR_COUNT=11;
st::Sensor *sensors[SENSOR_COUNT];

//test sketch
void setup()
{
  st::PollingSensor::DEBUG=true;
  st::InterruptSensor::DEBUG=true;
  Serial.begin(9600);
  sensors[0]=new st::PollingSensor("Temperature", 10000);
  sensors[1]=new st::PollingSensor("Humidity", 20000);
  sensors[2]=new st::PollingSensor("GarageDoor1", 4000);
  sensors[3]=new st::PollingSensor("GarageDoor2", 4000, 2000);
  sensors[4]=new st::PollingSensor("LightSensor", 30000);
  sensors[5]=new st::InterruptSensor("GarageButton", 8, HIGH);
  sensors[6]=new st::InterruptSensor("Doorbell", 9, HIGH);
  sensors[7]=new st::InterruptSensor("Alarm", 10, HIGH);
  sensors[8]=new st::InterruptSensor("Alert", 11, HIGH);
  sensors[9]=new st::InterruptSensor("GarbageDisposal", 12, HIGH);
  sensors[10]=new st::PS_Illuminance("illuminance", 1000, 0, A5);
}

void loop()
{
  for(unsigned int count=0; count<SENSOR_COUNT; ++count)
  {
     sensors[count]->update(); 
  }
}
