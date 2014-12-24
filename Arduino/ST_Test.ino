#include <SoftwareSerial.h>
#include <SmartThings.h>
#include <Constants.h>
#include <Device.h>
#include <Everything.h>
#include <Executor.h>
#include <InterruptSensor.h>
#include <PollingSensor.h>
#include <PS_Illuminance.h>
#include <Sensor.h>


st::PollingSensor test0(0, 10000, -2000);
st::PollingSensor test1(1, 10000, -6000);
st::PollingSensor test2(2, 10000, -4000);
st::PollingSensor test3(3, 10000, -1000);
st::PollingSensor test4(4, 10000, 12000);
st::InterruptSensor test5(5, 8, HIGH);
st::InterruptSensor test6(6, 9, HIGH);
st::PS_Illuminance test7(7, 10000, -8000, A5);
st::PollingSensor test8(8, 6000, 0);
st::PollingSensor test9(9, 6000, 3000);
st::InterruptSensor test10(5, 8, HIGH);
st::InterruptSensor test11(6, 9, HIGH);
st::PS_Illuminance test12(7, 10000, -4000, A5);
st::PollingSensor test13(8, 6000, 10000);
st::PollingSensor test14(9, 10000, 3000);

//test sketch
void setup()
{
  Serial.begin(9600); //THIS MUST BE CALLED FIRST TO AVOID TERRIBLE RESULTS
  st::Everything::debug=true;
  st::Device::debug=true;
  st::PollingSensor::debug=true;
  st::InterruptSensor::debug=true;
  st::Everything::Return_String.reserve(250);
  
  //st::Everything::initSmartThings();
  
  st::Everything::addSensor(&test0);
  st::Everything::addSensor(&test1); 
  st::Everything::addSensor(&test2); 
  st::Everything::addSensor(&test3); 
  st::Everything::addSensor(&test4); 
  st::Everything::addSensor(&test5); 
  st::Everything::addSensor(&test6);
  st::Everything::addSensor(&test7); 
  st::Everything::addSensor(&test8); 
  st::Everything::addSensor(&test9);
  st::Everything::addSensor(&test10); 
  st::Everything::addSensor(&test11);
  st::Everything::addSensor(&test12); 
  st::Everything::addSensor(&test13); 
  st::Everything::addSensor(&test14); 
  
  st::Everything::initDevices();
}

void loop()
{
  st::Everything::run();
}
