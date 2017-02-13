//******************************************************************************************
//  File: ST_Anything_Doors_Windows_ThingShield.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one Arduino + SmartThings Shield to 
//            implement a multi input/output custom device for integration into SmartThings.
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the SmartThings Shield.
//
//            ST_Anything_Doors_Example implements the following:
//              - 13 x Contact Sensor devices (used to monitor magnetic door/windows sensors)
//              - 1 x Motion device (used to detect motion)
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
//    2015-10-31  Dan Ogorchock  Revised for a specific user request
//    2017-02-12  Dan Ogorchock  Revised to use the new SMartThings v2.0 library
//
//******************************************************************************************

//******************************************************************************************
// SmartThings Library for Arduino Shield
//******************************************************************************************
#include <SmartThingsThingShield.h>    //Library to provide API to the SmartThings ThingShield

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

//Window Pins
#define PIN_CONTACT_KITCHEN_WINDOW1  4
#define PIN_CONTACT_KITCHEN_WINDOW2  5
#define PIN_CONTACT_KITCHEN_WINDOW3  7
#define PIN_CONTACT_MASTER_WINDOW1   8
#define PIN_CONTACT_MASTER_WINDOW2   9
#define PIN_CONTACT_OFFICE_WINDOW1   10
#define PIN_CONTACT_OFFICE_WINDOW2   11
#define PIN_CONTACT_GUEST_WINDOW1    12
#define PIN_CONTACT_GUEST_WINDOW2    13


//House Door Pins
#define PIN_CONTACT_FRONT_DOOR       A1
#define PIN_CONTACT_KITCHEN_DOOR     A2
#define PIN_CONTACT_GARAGE_DOOR      A3
#define PIN_CONTACT_BEDROOM_DOOR     A4

//motion pins
#define PIN_MOTION                   A5

//If using SoftwareSerial (e.g. Arduino UNO), must define pins for transmit and receive
#define pinRX 3
#define pinTX 2

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
 
  static st::IS_Motion sensor1(F("motion"), PIN_MOTION, HIGH, false);
  static st::IS_Contact sensor2(F("kitchenWindow1"), PIN_CONTACT_KITCHEN_WINDOW1, LOW, true, 500);
  static st::IS_Contact sensor3(F("kitchenWindow2"), PIN_CONTACT_KITCHEN_WINDOW2, LOW, true, 500);
  static st::IS_Contact sensor4(F("kitchenWindow3"), PIN_CONTACT_KITCHEN_WINDOW3, LOW, true, 500);
  static st::IS_Contact sensor5(F("masterWindow1"), PIN_CONTACT_MASTER_WINDOW1, LOW, true, 500);
  static st::IS_Contact sensor6(F("masterWindow2"), PIN_CONTACT_MASTER_WINDOW2, LOW, true, 500);
  static st::IS_Contact sensor7(F("officeWindow1"), PIN_CONTACT_OFFICE_WINDOW1, LOW, true, 500);
  static st::IS_Contact sensor8(F("officeWindow2"), PIN_CONTACT_OFFICE_WINDOW2, LOW, true, 500);
  static st::IS_Contact sensor9(F("guestWindow1"), PIN_CONTACT_GUEST_WINDOW1, LOW, true, 500);
  static st::IS_Contact sensor10(F("guestWindow2"), PIN_CONTACT_GUEST_WINDOW2, LOW, true, 500);
  static st::IS_Contact sensor11(F("frontDoor"), PIN_CONTACT_FRONT_DOOR, LOW, true, 500);
  static st::IS_Contact sensor12(F("kitchenDoor"), PIN_CONTACT_KITCHEN_DOOR, LOW, true, 500);
  static st::IS_Contact sensor13(F("garageDoor"), PIN_CONTACT_GARAGE_DOOR, LOW, true, 500);
  static st::IS_Contact sensor14(F("bedroomDoor"), PIN_CONTACT_BEDROOM_DOOR, LOW, true, 500);
  
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
  //Create the SmartThings Thingshield Communications Object based on Arduino Model
  #if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_MINI)   //Arduino UNO, NANO, MINI
    st::Everything::SmartThing = new st::SmartThingsThingShield(pinRX, pinTX, st::receiveSmartString);      //Use Software Serial
  #elif defined(ARDUINO_AVR_LEONARDO)  //Arduino Leonardo
    st::Everything::SmartThing = new st::SmartThingsThingShield(&Serial1, st::receiveSmartString);  //Use Hardware Serial
  #elif defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560)  //Arduino MEGA 1280 or 2560
    st::Everything::SmartThing = new st::SmartThingsThingShield(&Serial3, st::receiveSmartString);  //Use Hardware Serial
  #else  
    //assume user is using an UNO for the unknown case
    st::Everything::SmartThing = new st::SmartThingsThingShield(pinRX, pinTX, st::receiveSmartString);               //Software Serial
  #endif  

  //Run the Everything class' init() routine which establishes communications with SmartThings
  st::Everything::init();
  
  //*****************************************************************************
  //Add each sensor to the "Everything" Class
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
  
  //*****************************************************************************
  //Initialize each of the devices which were added to the Everything Class
  //*****************************************************************************
  st::Everything::initDevices();

  digitalWrite(A1, HIGH); // set pullup on analog pin 1
  digitalWrite(A2, HIGH); // set pullup on analog pin 2
  digitalWrite(A3, HIGH); // set pullup on analog pin 3
  digitalWrite(A4, HIGH); // set pullup on analog pin 4
  digitalWrite(A5, HIGH); // set pullup on analog pin 5

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
