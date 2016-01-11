//******************************************************************************************
//  File: ST_Anything_Alarm_Panel.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one Arduino + SmartThings Shield to 
//            implement a multi input/output custom device for integration into SmartThings.
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the SmartThings Shield.
//
//            ST_Anything_Alarm_Panel implements the following:
//              - 10 x Contact Sensor devices (used to monitor magnetic door/windows sensors)
//              - 2 x Motion device (used to detect motion)
//              - 3 x Smoke Detectors
//              - 1 x Siren
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
//    2015-01-07  Dan Ogorchock  Modified for Door Monitoring and Garage Door Control
//    2015-03-28  Dan Ogorchock  Removed RCSwitch #include now that the libraries are split up
//    2015-03-31  Daniel O.      Memory optimizations utilizing progmem
//    2015-12-06  Dan Ogorchock  Revised to add Smoke Detector Capability
//
//******************************************************************************************

//******************************************************************************************
// SmartThings Library for Arduino Shield
//******************************************************************************************
#include <SoftwareSerial.h> //Arduino UNO/Leonardo uses SoftwareSerial for the SmartThings Library
#include <SmartThings.h>    //Library to provide API to the SmartThings Shield
#include <avr/pgmspace.h>

//******************************************************************************************
// ST_Anything Library 
//******************************************************************************************
#include <Constants.h>       //Constants.h is designed to be modified by the end user to adjust behavior of the ST_Anything library
#include <Device.h>          //Generic Device Class, inherited by Sensor and Executor classes
#include <Sensor.h>          //Generic Sensor Class, typically provides data to ST Cloud (e.g. Temperature, Motion, etc...)
#include <InterruptSensor.h> //Generic Interrupt "Sensor" Class, waits for change of state on digital input 
#include <Everything.h>      //Master Brain of ST_Anything library that ties everything together and performs ST Shield communications

#include <IS_Motion.h>       //Implements an Interrupt Sensor (IS) to detect motion via a PIR sensor
#include <IS_Contact.h>      //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin
#include <IS_Smoke.h>        //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin
#include <EX_Alarm.h>        //Implements Executor (EX)as an Alarm Siren capability via a digital output to a relay

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


//Mmotion Detector Pins
#define PIN_MOTION_1                 22
#define PIN_MOTION_2                 23

//Contact Sensor Pins
#define PIN_CONTACT_1                26
#define PIN_CONTACT_2                27
#define PIN_CONTACT_3                28
#define PIN_CONTACT_4                29
#define PIN_CONTACT_5                30
#define PIN_CONTACT_6                31
#define PIN_CONTACT_7                32
#define PIN_CONTACT_8                33
#define PIN_CONTACT_9                34
#define PIN_CONTACT_10               35

//Smoke Detector Pins
#define PIN_SMOKE_1                  40
#define PIN_SMOKE_2                  41
#define PIN_SMOKE_3                  42

//Alarm Siren Pin
#define PIN_ALARM                    50


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
 
  static st::IS_Motion sensor1(F("motion1"), PIN_MOTION_1, HIGH, false);
  static st::IS_Motion sensor2(F("motion2"), PIN_MOTION_2, HIGH, false);
  
  static st::IS_Contact sensor3(F("contact1"), PIN_CONTACT_1, LOW, true, 500);
  static st::IS_Contact sensor4(F("contact2"), PIN_CONTACT_2, LOW, true, 500);
  static st::IS_Contact sensor5(F("contact3"), PIN_CONTACT_3, LOW, true, 500);
  static st::IS_Contact sensor6(F("contact4"), PIN_CONTACT_4, LOW, true, 500);
  static st::IS_Contact sensor7(F("contact5"), PIN_CONTACT_5, LOW, true, 500);
  static st::IS_Contact sensor8(F("contact6"), PIN_CONTACT_6, LOW, true, 500);
  static st::IS_Contact sensor9(F("contact7"), PIN_CONTACT_7, LOW, true, 500);
  static st::IS_Contact sensor10(F("contact8"), PIN_CONTACT_8, LOW, true, 500);
  static st::IS_Contact sensor11(F("contact9"), PIN_CONTACT_9, LOW, true, 500);
  static st::IS_Contact sensor12(F("contact10"), PIN_CONTACT_10, LOW, true, 500);
  
  static st::IS_Smoke sensor13(F("smoke1"), PIN_SMOKE_1, HIGH, true, 500);
  static st::IS_Smoke sensor14(F("smoke2"), PIN_SMOKE_2, HIGH, true, 500);
  static st::IS_Smoke sensor15(F("smoke3"), PIN_SMOKE_3, HIGH, true, 500);
 
  static st::EX_Alarm executor1(F("alarm"), PIN_ALARM, LOW, true);
  //*****************************************************************************
  //  Configure debug print output from each main class 
  //  -Note: Set these to "false" if using Hardware Serial on pins 0 & 1
  //         to prevent communication conflicts with the ST Shield communications
  //*****************************************************************************
  st::Everything::debug=true;
  st::Device::debug=true;
  st::InterruptSensor::debug=true;
  
  //*****************************************************************************
  //Initialize the "Everything" Class
  //*****************************************************************************
  st::Everything::init();
  
  //*****************************************************************************
  //Add each sensor (i.e inputs) to the "Everything" Class
  //*****************************************************************************
  st::Everything::addSensor(&sensor1);
  st::Everything::addSensor(&sensor2);
  st::Everything::addSensor(&sensor3);
  st::Everything::addSensor(&sensor4); 
  st::Everything::addSensor(&sensor5); 
  st::Everything::addSensor(&sensor6);
  st::Everything::addSensor(&sensor7); 
  st::Everything::addSensor(&sensor8);
  st::Everything::addSensor(&sensor9);
  st::Everything::addSensor(&sensor10); 
  st::Everything::addSensor(&sensor11); 
  st::Everything::addSensor(&sensor12);
  st::Everything::addSensor(&sensor13); 
  st::Everything::addSensor(&sensor14);
  st::Everything::addSensor(&sensor15);
  
  //*****************************************************************************
  //Add each executor (i.e. ouputs) to the "Everything" Class
  //*****************************************************************************
  st::Everything::addExecutor(&executor1);
  
  //*****************************************************************************
  //Initialize each of the devices which were added to the Everything Class
  //*****************************************************************************
  st::Everything::initDevices();
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
