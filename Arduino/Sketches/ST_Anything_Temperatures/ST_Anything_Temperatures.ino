//******************************************************************************************
//  File: ST_Anything_Temperatures.ino
//  Authors: Dan G Ogorchock
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
///              ______________
///             |              |
///             |         SW[] |
///             |[]RST         |
///             |         AREF |--
///             |          GND |--
///             |           13 |--X Othercrisp DHT
///             |           12 |--X Moistcrisp DHT
///             |           11 |--X Fridge DHT
///           --| 3.3V      10 |--X Freezer DHT
///           --| 5V         9 |--X CS
///           --| GND        8 |--THING_RX ------------------|
///           --| GND          |                             |
///           --| Vin        7 |--X CLK                      |
///             |            6 |--reserved by ThingShield    |
///           --| A0         5 |--X DO for Broiler           |
///           --| A1    ( )  4 |--X DO for Oven              |
///           --| A2         3 |--X THING_RX ----------------|
///           --| A3  ____   2 |--X THING_TX
///           --| A4 |    |  1 |--
///           --| A5 |    |  0 |--
///             |____|    |____|
///                  |____|
//    
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//    2015-03-24  Dan Ogorchock  Modified to just monitor multiple DTHT Temp/Humid sensors
//
//
//******************************************************************************************

//******************************************************************************************
// SmartThings Library for Arduino Shield
//******************************************************************************************
#include <SoftwareSerial.h>     //Arduino UNO/Leonardo uses SoftwareSerial for the SmartThings Library
#include <SmartThings.h>        //Library to provide API to the SmartThings Shield
#include <dht.h>                //DHT Temperature and Humidity Library (*** Rob Tillaart's version ***)
#include <SPI.h>                //thermocouple additions
#include <Adafruit_MAX31855.h>  //thermocouple additions
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

#include <PS_TemperatureHumidity.h>  //Implements a Polling Sensor (PS) to measure Temperature and Humidity via DHT library


//******************************************************************************************
//Define which Arduino Pins will be used for each device
//  Notes: -Serial Communications Pins are defined in Constants.h (avoid pins 0,1,2,3
//          for inputs and output devices below as they may be used for communications)
//         -Always avoid Pin 6 as it is reserved by the SmartThings Shield
//
//******************************************************************************************
#define PIN_TEMPHUMID_FREEZER     10
#define PIN_TEMPHUMID_FRIDGE      11
#define PIN_TEMPHUMID_MOISTCRISP  12
#define PIN_TEMPHUMID_OTHERCRISP  13

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
//
// TODO: Customize based on what devices the user needs attached to the Arduino
//
//******************************************************************************************
//Polling Sensors
st::PS_TemperatureHumidity sensor1("th_freezer", 10, 0, PIN_TEMPHUMID_FREEZER, st::PS_TemperatureHumidity::DHT22, "t_Freezer", "h_Freezer");
st::PS_TemperatureHumidity sensor2("th_fridge", 10, 2, PIN_TEMPHUMID_FRIDGE, st::PS_TemperatureHumidity::DHT22, "t_Fridge", "h_Fridge");
st::PS_TemperatureHumidity sensor3("th_moistcrisp", 10, 4, PIN_TEMPHUMID_MOISTCRISP, st::PS_TemperatureHumidity::DHT22, "t_Moistcrisp", "h_Moistcrisp");
st::PS_TemperatureHumidity sensor4("th_othercrisp", 10, 6, PIN_TEMPHUMID_OTHERCRISP, st::PS_TemperatureHumidity::DHT22, "t_Othercrisp", "h_Othercrisp");

//Interrupt Sensors 

//Executors

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
  // TODO: Customize based on sensors defined in global section above
  //*****************************************************************************
  st::Everything::addSensor(&sensor1);
  st::Everything::addSensor(&sensor2);
  st::Everything::addSensor(&sensor3);
  st::Everything::addSensor(&sensor4); 
  
  //*****************************************************************************
  //Add each executor to the "Everything" Class
  // TODO: Customize based on executors defined in global section above
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
