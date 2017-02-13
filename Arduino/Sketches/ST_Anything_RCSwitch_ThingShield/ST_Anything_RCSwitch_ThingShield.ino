//******************************************************************************************
//  File: ST_Anything_RCSwitch_ThingShield.ino
//  Authors: Dan G Ogorchock 
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one Arduino + SmartThings Shield to 
//            implement an RCSwitch device which can be mapped back to a SmartThings "Switch"
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
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//    2015-01-26  Dan            Added RCSwitch support/example
//    2015-01-30  Dan            Cleaned up specificailly for RCSwitch use exclusively for size
//    2015-03-31  Daniel O.      Memory optimizations utilizing progmem
//    2015-05-20  Dan            Improved capability and modified for Etekcity ZAP 3F example
//    2017-02-12  Dan Ogorchock  Revised to use the new SMartThings v2.0 library
//
//******************************************************************************************

//******************************************************************************************
// SmartThings Library for Arduino Shield
//******************************************************************************************
#include <SmartThingsThingShield.h>    //Library to provide API to the SmartThings Shield

//******************************************************************************************
// ST_Anything Library 
//******************************************************************************************
#include <Constants.h>       //Constants.h is designed to be modified by the end user to adjust behavior of the ST_Anything library
#include <Device.h>          //Generic Device Class, inherited by Sensor and Executor classes
#include <Sensor.h>          //Generic Sensor Class, typically provides data to ST Cloud (e.g. Temperature, Motion, etc...)
#include <Executor.h>        //Generic Executor Class, typically receives data from ST Cloud (e.g. Switch)
#include <Everything.h>      //Master Brain of ST_Anything library that ties everything together and performs ST Shield communications

#include <EX_RCSwitch.h>     //Implements the Executor (EX) as an RCSwitch object

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

#define PIN_RCSWITCH                 10

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
  //           main comments in the corresponding header (.h) and program (.cpp) files.
  //         - The name assigned to each device (1st argument below) must match the Groovy
  //           DeviceType Tile name in your custom DeviceType code. 
  //******************************************************************************************
  //Polling Sensors
  
  //Interrupt Sensors 
  
  //Executors
  static st::EX_RCSwitch executor1(F("rcswitch1"), PIN_RCSWITCH, 5526835, 24, 5526844, 24, 174, 1, 15, LOW);
  static st::EX_RCSwitch executor2(F("rcswitch2"), PIN_RCSWITCH, 5526979, 24, 5526988, 24, 174, 1, 15, LOW);
  static st::EX_RCSwitch executor3(F("rcswitch3"), PIN_RCSWITCH, 5527299, 24, 5527308, 24, 174, 1, 15, LOW);

  //*****************************************************************************
  //  Configure debug print output from each main class 
  //  -Note: Set these to "false" if using Hardware Serial on pins 0 & 1
  //         to prevent communication conflicts with the ST Shield communications
  //*****************************************************************************
  st::Everything::debug=true;
  st::Executor::debug=true;
  st::Device::debug=true;
  
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
  
  //*****************************************************************************
  //Add each executor to the "Everything" Class
  //*****************************************************************************
  st::Everything::addExecutor(&executor1);
  st::Everything::addExecutor(&executor2);
  st::Everything::addExecutor(&executor3);
  
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
