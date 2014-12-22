#include <Device.h>
#include <Sensor.h>
#include <PollingSensor.h>

st::PollingSensor *ps;


void setup()
{
  st::PollingSensor::DEBUG=true;
  Serial.begin(9600);
  ps=new st::PollingSensor("Daniel", 1000);
}

void loop()
{
  ps->update();
}
