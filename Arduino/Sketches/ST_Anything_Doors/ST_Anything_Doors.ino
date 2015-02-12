//******************************************************************************************
//  File: ST_Anything_Doors.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one Arduino + SmartThings Shield to 
//            implement a multi input/output custom device for integration into SmartThings.
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the SmartThings Shield.
//
//            ST_Anything_Doors implements the following:
//              - 2 x Door Control devices (used as Garage Doors)
//              - 4 x Contact Sensor devices (used to monitor magnetic door sensors)
//              - 1 x Motion device (used to detect motion in the garage)
//              - 1 x Temperature/Humidity device (unsed to monitor temp & humidity in the garage)
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
//
//
//******************************************************************************************

//******************************************************************************************
// SmartThings Library for Arduino Shield
//******************************************************************************************
#include <SoftwareSerial.h> //Arduino UNO/Leonardo uses SoftwareSerial for the SmartThings Library
#include <SmartThings.h>    //Library to provide API to the SmartThings Shield
#include <dht.h>            //DHT Temperature and Humidity Library 
#include <RCSwitch.h>       //Library to provide support for RCSwitch devices
//******************************************************************************************
// ST_Anything Library 
//******************************************************************************************
#include <Constants.h>       //Constants.h is designed to be modified by the end user to adjust behavior of the ST_Anything library
#include <Device.h>          //Generic Device Class, inherited by Sensor and Executor classes
#include <Sensor.h>          //Generic Sensor Class, typically provides data to ST Cloud (e.g. Temperature, Motion, etc...)
#include <InterruptSensor.h> //Generic Interrupt "Sensor" Class, waits for change of state on digital input 
#include <PollingSensor.h>   //Generic Polling "Sensor" Class, polls Arduino pins periodically
#include <Everything.h>      //Master Brain of ST_Anything library that ties everything together and performs ST Shield communications


#include <PS_TemperatureHumidity.h>  //Implements a Polling Sensor (PS) to measure Temperature and Humidity via DHT library
#include <IS_Motion.h>       //Implements an Interrupt Sensor (IS) to detect motion via a PIR sensor
#include <IS_Contact.h>      //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin
#include <IS_DoorControl.h> //Implements an Interrupt Sensor (IS) and Executor to monitor the status of a digital input pin and control a digital output pin

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

//Motion and Temperature/Humidity Pins (Note: using Analog pins as Digital inputs)
#define PIN_MOTION                   A0
#define PIN_TEMPERATUREHUMIDITY      A1

//Garage Door Pins 
#define PIN_RELAY_LEFTGARAGE_DOOR    5
#define PIN_RELAY_RIGHTGARAGE_DOOR   7
#define PIN_CONTACT_RIGHTGARAGE_DOOR 8
#define PIN_CONTACT_LEFTGARAGE_DOOR  9

//House Door Pins
#define PIN_CONTACT_FRONT_DOOR       10
#define PIN_CONTACT_BACK_DOOR        11
#define PIN_CONTACT_KITCHEN_DOOR     12
#define PIN_CONTACT_SIDEGARAGE_DOOR  13

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
st::PS_TemperatureHumidity sensor1("temphumid", 120, 10, PIN_TEMPERATUREHUMIDITY, st::PS_TemperatureHumidity::DHT22);

//Interrupt Sensors 
st::IS_Motion sensor2("motion", PIN_MOTION, HIGH, false);
st::IS_DoorControl sensor3("leftDoor", PIN_CONTACT_LEFTGARAGE_DOOR, LOW, true, PIN_RELAY_LEFTGARAGE_DOOR, LOW, true, 1000);
st::IS_DoorControl sensor4("rightDoor", PIN_CONTACT_RIGHTGARAGE_DOOR, LOW, true, PIN_RELAY_RIGHTGARAGE_DOOR, LOW, true, 1000);
st::IS_Contact sensor5("garagesideDoor", PIN_CONTACT_SIDEGARAGE_DOOR, LOW, true);
st::IS_Contact sensor6("frontDoor", PIN_CONTACT_FRONT_DOOR, LOW, true);
st::IS_Contact sensor7("backDoor", PIN_CONTACT_BACK_DOOR, LOW, true);
st::IS_Contact sensor8("kitchenDoor", PIN_CONTACT_KITCHEN_DOOR, LOW, true);

//Executors
//st::EX_Switch executor1("sampleEX", PIN_sampleEX, LOW, true);


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
  st::Everything::addSensor(&sensor2);
  st::Everything::addSensor(&sensor3);
  st::Everything::addSensor(&sensor4); 
  st::Everything::addSensor(&sensor5); 
  st::Everything::addSensor(&sensor6);
  st::Everything::addSensor(&sensor7); 
  st::Everything::addSensor(&sensor8);
  
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
