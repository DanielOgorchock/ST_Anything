#include <Constants.h>
#include <Device.h>
#include <Everything.h>
#include <Executor.h>
#include <InterruptSensor.h>
#include <PollingSensor.h>
#include <PS_Illuminance.h>
#include <Sensor.h>

st::Everything everything;

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
  Serial.begin(9600); //THIS MUST BE CALLED FIRST TO AVOID TERRIBLE RESULTS
  
  st::Everything::debug=true;
  st::Device::debug=true;
  st::PollingSensor::debug=true;
  st::InterruptSensor::debug=true;
  
  everything.addSensor(&test0);
  everything.addSensor(&test1); 
  everything.addSensor(&test2); 
  everything.addSensor(&test3); 
  everything.addSensor(&test4); 
  everything.addSensor(&test5); 
  everything.addSensor(&test6);
  everything.addSensor(&test7); 
  everything.addSensor(new st::InterruptSensor("Alert", 11, HIGH));
  everything.addSensor(new st::InterruptSensor("GarbageDisposal", 12, HIGH));
  everything.addSensor(new st::PollingSensor("dynamicPolling", 2000, 0));
  
  everything.init();
}

void loop()
{
  everything.run();
}
