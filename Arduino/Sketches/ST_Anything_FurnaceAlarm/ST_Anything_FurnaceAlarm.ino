//******************************************************************************************
//  File: ST_Anything_FUrnaceAlarm.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one Arduino + SmartThings Shield to 
//            implement a multi input/output custom device for integration into SmartThings.
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
//    2015-01-03  Dan & Daniel   Original Creation
//    2015-03-10  Dan            Modified to monitor furnace alarm
//    2015-03-14  Dan            Added LED capability for visual status of the FurnaceAlarm
//
//
//******************************************************************************************

//******************************************************************************************
// SmartThings Library for Arduino Shield
//******************************************************************************************
#include <SoftwareSerial.h> //Arduino UNO/Leonardo uses SoftwareSerial for the SmartThings Library
#include <SmartThings.h>    //Library to provide API to the SmartThings Shield

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

#include <IS_Contact.h>      //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin

//******************************************************************************************
//Define which Arduino Pins will be used for each device
//  Notes: -Serial Communications Pins are defined in Constants.h (avoid pins 0,1,2,3
//          for inputs and output devices below as they may be used for communications)
//         -Always avoid Pin 6 as it is reserved by the SmartThings Shield
//
//******************************************************************************************

#define PIN_CONTACT              11

//******************************************************************************************
//Declare each Device that is attached to the Arduino
//  Notes: - For each device, there is typically a corresponding "tile" defined in your 
//           SmartThings DeviceType Groovy code
//         - For details on each device's constructor arguments below, please refer to the 
//           corresponding header (.h) and program (.cpp) files.
//         - The name assigned to each device (1st argument below) must match the Groovy
//           DeviceType Tile name.  (Note: "temphumid" below is the exception to this rule
//           as the DHT sensors produce both "temperature" and "humidity".  Data from that
//           particular sensor is sent to the ST Shield in two separate updates, one for 
//           "temperature" and one for "humidity")
//******************************************************************************************
//Polling Sensors

//Interrupt Sensors 
st::IS_Contact sensor1("contact", PIN_CONTACT, LOW, true);

//Executors


//Global Variables
bool lastStatus;  //Hold the last status of the Furnace Alarm to prevent updating the LED too frequently

//******************************************************************************************
//Arduino Setup() routine
//******************************************************************************************
void setup()
{
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
  
  //*****************************************************************************
  //Initialize each of the devices which were added to the Everything Class
  st::Everything::initDevices();
  //*****************************************************************************
  
  //Set the lastStatus variable to force the LED update the first time through the loop()
  lastStatus = !sensor1.getStatus();
  
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
  
  //Check to see if the Furnace is in Alarm.  If true, set the Smart Thing Shield
  //LED to Red.  If false, set the LED to Green.
  if (sensor1.getStatus() && !lastStatus)
  {
      //ALARM!!! (Red)
      st::Everything::setLED(2,0,0);
      lastStatus = true;

  }
  else if (!sensor1.getStatus() && lastStatus)
  {
      //NORMAL (Green)
      st::Everything::setLED(0,2,0);  
      lastStatus = false;
  }
  
}
