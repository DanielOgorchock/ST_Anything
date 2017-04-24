//******************************************************************************************
//  File: ST_Anything_Power_ThingShield.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one Arduino + SmartThings Shield to 
//            implement a pulse counting custom device for integration into SmartThings.  It implements 
//            SmartThings "Power Meter" capability since that is the closest capability to the
//            intended use.
//
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the SmartThings Shield.
//
// ********** This sketch requires an Arduino MEGA 2560 as it requires an available pin that
// *  NOTE! * supports External Hardware Interrupts.  Only Pins 21, 20, 19, and 18 on the MEGA
// ********** are valid since Pins 2 and 3 are already used by the SmartThings ThingShield.
//
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
//    2015-01-03  Dan & Daniel   Original Creation (ST_Anything demo)
//    2015-03-31  Dan Ogorchock  Modified for a Pulse Counter / Energy Meter
//    2015-04-14  Daniel O.      Support for memory optimizations
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
#include <InterruptSensor.h> //Generic Interrupt "Sensor" Class, waits for change of state on digital input 
#include <PollingSensor.h>   //Generic Polling "Sensor" Class, polls Arduino pins periodically
#include <Everything.h>      //Master Brain of ST_Anything library that ties everything together and performs ST Shield communications

#include <PS_PulseCounter.h> //Implements a Polling Sensor (PS) to measure pulse counts via HW Interrupt

//******************************************************************************************
//Define which Arduino Pins will be used for each device
//  Notes: -Serial Communications Pins are defined in Constants.h (avoid pins 0,1,2,3
//          for inputs and output devices below as they may be used for communications)
//         -Always avoid Pin 6 as it is reserved by the SmartThings Shield
//
//******************************************************************************************
//"RESERVED" pins for SmartThings ThingShield - best to avoid
#define PIN_O_RESERVED  0  //reserved by ThingShield for Serial communications OR USB Serial Monitor
#define PIN_1_RESERVED  1  //reserved by ThingShield for Serial communications OR USB Serial Monitor
#define PIN_2_RESERVED  2  //reserved by ThingShield for Serial communications
#define PIN_3_RESERVED  3  //reserved by ThingShield for Serial communications
#define PIN_6_RESERVED  6  //reserved by ThingShield (possible future use?)

//User defintions
#define PULSE_PIN         21  //Must use Pin 21, 20, 19, or 18 as these corrspond to Interrupts 2, 3, 4, 5 respectively
#define PULSE_POLL_INTRVL 30  //Polling Interval in seconds (how often to send data to SmartThings)
#define PULSE_POLL_OFFSET 0   //Polling Offset in seconds (phasing time shift offset to prevent all sensors sending at once)
#define PULSE_INTTYPE     FALLING      //Interrupt Type (FALLING, RISING, CHANGE)
#define PULSE_INPUTMODE   INPUT_PULLUP //Input Mode for the interrupt pin (INPUT_PULLUP, INPUT)
#define PULSE_SLOPE       1.0  //Linear conversion slope  (eng units = PULSE_SLOPE * counts + PULSE_OFFSET)
#define PULSE_OFFSET      0.0  //Linear conversion offset (eng units = PULSE_SLOPE * counts + PULSE_OFFSET)

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
  //           DeviceType Tile name.  (Note: "temphumid" below is the exception to this rule
  //           as the DHT sensors produce both "temperature" and "humidity".  Data from that
  //           particular sensor is sent to the ST Shield in two separate updates, one for 
  //           "temperature" and one for "humidity")
  //******************************************************************************************
  //Polling Sensors
  static st::PS_PulseCounter sensor1(F("power"), PULSE_POLL_INTRVL, PULSE_POLL_OFFSET, PULSE_PIN, PULSE_INTTYPE, PULSE_INPUTMODE, PULSE_SLOPE, PULSE_OFFSET);
  
  //Interrupt Sensors 
  
  //Executors

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
  
  
  //*****************************************************************************
  //Add each executor to the "Everything" Class
  //*****************************************************************************
  
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
