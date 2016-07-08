//******************************************************************************************
//  File: ST_Anything_Doors_Windows.ino
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
#include <PollingSensor.h>   //Generic Polling "Sensor" Class, polls Arduino pins periodically
#include <Everything.h>      //Master Brain of ST_Anything library that ties everything together and performs ST Shield communications

#include <IS_Motion.h>       //Implements an Interrupt Sensor (IS) to detect motion via a PIR sensor
#include <IS_Contact.h>      //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin
#include <S_TimedRelay.h> //Implements an Interrupt Sensor (IS) and Executor to monitor the status of a digital input pin and control a digital output pin

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

//Door/Window Chime Pin
#define PIN_RELAY                    A0

//motion pins
#define PIN_MOTION                   A5



//******************************************************************************************
//st::Everything::callOnMsgSend() optional callback routine.  This is a sniffer to monitor 
//    data being sent to ST.  This allows a user to act on data changes locally within the 
//    Arduino sktech.
//******************************************************************************************
void callback(const String &msg)
{
  Serial.print(F("ST_Anything_wCallback: Sniffed data = "));
  Serial.println(msg);
  
  //If any door or windows is opened, then start timedRelay output sequence
  if (msg.endsWith("open"))
  {
    //Masquerade as the ThingShield to "send data to the Arduino, as if from the ST Cloud"
    st::receiveSmartString("switch on");  //use same strings that the DeviceHandler would send
  }
  
}

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


     //******************************************************************************************
    //       st::S_TimedRelay() constructor requires the following arguments
    //        - String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
    //        - byte pinOutput - REQUIRED - the Arduino Pin to be used as a digital output
    //        - bool startingState - REQUIRED - the value desired for the initial state of the switch.  LOW = "off", HIGH = "on"
    //        - bool invertLogic - REQUIRED - determines whether the Arduino Digital Ouput should use inverted logic
    //        - long onTime - OPTIONAL - the number of milliseconds to keep the output on, DEFGAULTS to 1000 milliseconds
    //        - long offTime - OPTIONAL - the number of milliseconds to keep the output off, DEFAULTS to 0
    //        - intnumCycles - OPTIONAL - the number of times to repeat the on/off cycle, DEFAULTS to 1
    //******************************************************************************************
  static st::S_TimedRelay sensor15(F("switch"), PIN_RELAY, LOW, true, 1000, 0, 1);

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
  //  Configure callback function if desired 
  //*****************************************************************************
  st::Everything::callOnMsgSend = callback;
  
 
  //*****************************************************************************
  //Initialize the "Everything" Class
  //*****************************************************************************
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
  st::Everything::addSensor(&sensor15);
  
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
