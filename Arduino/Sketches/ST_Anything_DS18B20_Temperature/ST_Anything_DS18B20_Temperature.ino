//******************************************************************************************
//  File: ST_Anything_DS18B20_Temperature.ino
//  Authors: Dan G Ogorchock
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one Arduino + SmartThings Shield to 
//            implement a DS18B20 Temperature Sensor device for integration into SmartThings.
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the SmartThings Shield.
//
//            During the development of this re-usable library, it became apparent that the 
//            Arduino UNO R3's very limited 2K of SRAM was very limiting in the number of 
//            devices that could be implemented simultaneously.  A tremendous amount of effort
//            has gone into reducing the SRAM usage, including siginificant improvements to
//            the SmartThings Arduino library.  The SmartThings library was also modified to
//            include support for using Hardware Serial port(s) on the UNO, MEGA, and Leonardo.
//            During testing, it was determined that the Hardware Serial ports provide much
//            better performance and reliability versus the SoftwareSerial library.  Also, the
//            MEGA 2560's 8K of SRAM is well worth the few extra dollars to save your sanity
//            versus always running out of SRAM on the UNO R3.  The MEGA 2560 also has 4 Hardware
//            serial ports (i.e. UARTS) which makes it very easy to use Hardware Serial instead 
//            of SoftwareSerial, while still being able to see debug data on the USB serial 
//            console port (pins 0 & 1).  
//
//            Note: We did not have a Leonardo for testing, but did fully test on UNO R3 and 
//            MEGA 2560 using both SoftwareSerial and Hardware Serial communications to the 
//            Thing Shield.
//    
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2016-02-19  Dan Ogorchock  Created example for Dallas Semiconductor 1-Wire DS18B20 Temperature Sensor
//
//******************************************************************************************
//******************************************************************************************
// SmartThings Library for Arduino Shield
//******************************************************************************************
#include <SoftwareSerial.h> //Arduino UNO/Leonardo uses SoftwareSerial for the SmartThings Library
#include <SmartThings.h>    //Library to provide API to the SmartThings Shield
#include <DallasTemperature.h>
#include <Wire.h>
#include <OneWire.h>

#include <avr/pgmspace.h>

//******************************************************************************************
// ST_Anything Library 
//******************************************************************************************
#include <Constants.h>       //Constants.h is designed to be modified by the end user to adjust behavior of the ST_Anything library
#include <Device.h>          //Generic Device Class, inherited by Sensor and Executor classes
#include <Sensor.h>          //Generic Sensor Class, typically provides data to ST Cloud (e.g. Temperature, Motion, etc...)
#include <Executor.h>        //Generic Executor Class, typically receives data from ST Cloud (e.g. Switch)
#include <InterruptSensor.h> //Generic Interrupt "Sensor" Class, waits for change of state on digital input 
#include <PollingSensor.h>   //Generic Polling "Sensor" Class, polls Arduino pins periodically
#include <Everything.h>      //Master Brain of ST_Anything library that ties everything together and performs ST Shield communications

#include <PS_DS18B20_Temperature.h>  //Implement a Polling Sensor (PS) to measure Temperature via DS18B20

//******************************************************************************************
//Define which Arduino Pins will be used for each device
//  Notes: -Serial Communications Pins are defined in Constants.h (avoid pins 0,1,2,3
//          for inputs and output devices below as they may be used for communications)
//         -Always avoid Pin 6 as it is reserved by the SmartThings Shield
//
//******************************************************************************************
//"RESERVED" pins for SmartThings ThingShield - best to avoid
#define PIN_O_RESERVED               0  //reserved by ThingShield for Serial communications OR USB Serial Monitor
#define PIN_1_RESERVED               1  //reserved by ThingShield for Serial communications OR USB Serial Monitor
#define PIN_2_RESERVED               2  //reserved by ThingShield for Serial communications
#define PIN_3_RESERVED               3  //reserved by ThingShield for Serial communications
#define PIN_6_RESERVED               6  //reserved by ThingShield (possible future use?)

#define PIN_DS18B20_Temperature      8

//******************************************************************************************
//Arduino Setup() routine
//******************************************************************************************
void setup()
{
  //******************************************************************************************
  //Declare each Device that is attached to the Arduino
  //  Notes: - For each device, there is typically a corresponding "tile" defined in your 
  //           SmartThings DeviceType Groovy code
  //         - For details on each device's constructor arguments below, please refer to the 
  //           corresponding header (.h) and program (.cpp) files.
  //         - The name assigned to each device (1st argument below) must match the Groovy
  //           DeviceType Tile name.  
  //******************************************************************************************
  //Polling Sensors
  static st::PS_DS18B20_Temperature sensor1(F("temperature"), 120, 0, PIN_DS18B20_Temperature, false, 10);
 
  //Interrupt Sensors 
  
  
  //*****************************************************************************
  //  Configure debug print output from each main class 
  //  -Note: Set these to "false" if using Hardware Serial on pins 0 & 1
  //         to prevent communication conflicts with the ST Shield communications
  //*****************************************************************************
  st::Everything::debug=true;
  st::Executor::debug=true;
  st::Device::debug=true;
  st::PollingSensor::debug=true;
  st::InterruptSensor::debug=true;
  
  //*****************************************************************************
  //Initialize the "Everything" Class
  //*****************************************************************************
  st::Everything::init();
  
  //*****************************************************************************
  //Add each sensor to the "Everything" Class
  //*****************************************************************************
  st::Everything::addSensor(&sensor1);
  
  //*****************************************************************************
  //Add each executor to the "Everything" Class
  //*****************************************************************************
  //st::Everything::addExecutor(&executor1);
  
  //*****************************************************************************
  //Initialize each of the devices which were added to the Everything Class
  st::Everything::initDevices();
  //*****************************************************************************
}

//******************************************************************************************
//Arduino Loop() routine
//******************************************************************************************
void loop()
{
  //*****************************************************************************
  //Execute the Everything run method which takes care of "Everything"
  //*****************************************************************************
  st::Everything::run();
}
