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

int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

st::PollingSensor test0("bogus_name__", 100000, -2000);
st::PollingSensor test1("bogus_name__", 100000, -6000);
st::PollingSensor test2("bogus_name__", 100000, -4000);
st::PollingSensor test3("bogus_name__", 100000, -1000);
st::PollingSensor test4("bogus_name__", 100000, 12000);
st::InterruptSensor test5("bogus_name__", 8, HIGH);
st::InterruptSensor test6("bogus_name__", 9, HIGH);
st::PS_Illuminance test7("illuminance", 10000, -5000, A5);
st::PollingSensor test8("bogus_name__", 600000, 0);
st::PollingSensor test9("bogus_name__", 600000, 3000);
st::InterruptSensor test10("bogus_name__", 8, HIGH);
st::InterruptSensor test11("bogus_name__", 9, HIGH);
st::PS_Illuminance test12("illuminance", 10000, 0, A5);
//st::PollingSensor test13("bogus_name__", 600000, 10000);
//st::PollingSensor test14("bogus_name__", 100000, 3000);

//test sketch
void setup()
{
  Serial.begin(9600); //THIS MUST BE CALLED FIRST TO AVOID TERRIBLE RESULTS
  st::Everything::debug=true;
  st::Device::debug=true;
  st::PollingSensor::debug=true;
  st::InterruptSensor::debug=true;
  st::Everything::Return_String.reserve(st::Constants::RETURN_STRING_RESERVE);
  
  Serial.println(freeRam());
  st::Everything::initSmartThings();
//  Serial.println(freeRam());
//  st::Everything::addSensor(new st::PollingSensor("0", 100000, -2000));
//  Serial.println(freeRam());
//  st::Everything::addSensor(new st::PollingSensor("1", 100000, -6000));
//  Serial.println(freeRam());  
//  st::Everything::addSensor(new st::PollingSensor("2", 100000, -4000));
//  Serial.println(freeRam());
//  st::Everything::addSensor(new st::PollingSensor("3", 100000, -1000));
//  Serial.println(freeRam());
//  st::Everything::addSensor(new st::PollingSensor("4", 100000, 12000));
//  Serial.println(freeRam());
//  st::Everything::addSensor(new st::InterruptSensor("5", 8, HIGH));
//  Serial.println(freeRam());
//  st::Everything::addSensor(new st::InterruptSensor("6", 9, HIGH));
//  Serial.println(freeRam());
//  st::Everything::addSensor(new st::PS_Illuminance("7", 10000, -5000, A5));
//  Serial.println(freeRam());
//  st::Everything::addSensor(new st::PollingSensor("8", 600000, 0));
//  Serial.println(freeRam());
//  st::Everything::addSensor(new st::PollingSensor("9", 600000, 3000));
  //st::Everything::addSensor(new st::InterruptSensor("10", 8, HIGH));
  //st::Everything::addSensor(new st::InterruptSensor("11", 9, HIGH));
  //st::Everything::addSensor(new st::PS_Illuminance("12", 10000, 0, A5));
  //st::Everything::addSensor(new st::PollingSensor("13", 600000, 10000));
  //st::Everything::addSensor(new st::PollingSensor("14", 100000, 3000));
  Serial.println(freeRam());
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
  //st::Everything::addSensor(&test13); 
  //st::Everything::addSensor(&test14); 
  Serial.println(freeRam());
  st::Everything::initDevices();
  Serial.println(freeRam());
}

void loop()
{
  st::Everything::run();
  if(millis()%1000==0)
    Serial.println(freeRam());
}
