//******************************************************************************************
//  File: ST_Anything_I2C_ESP8266WiFi.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one NodeMCU ESP8266 to 
//            implement a multi input/output custom device for integration into SmartThings/Hubitat.
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the NodeMCU ESP8266's WiFi.
//
//            ST_Anything_I2C implements the following ST Capabilities as a demo of I2C sensors on a NodeMCU ESP8266
//              - 1 x AM2320 Temperature and Humidity sensor
//              - 1 x BME280 Temperature, Humidity, and Pressure sensor (same I2C address as BMP280)
//              - 1 x BMP280 Temperature and Pressure sensor (same I2C address as BEP280)
//              - 1 x TCS34725 Color Illuminance sensor
//              - 1 x TSL2561 Illuminance sensor
//              - 1 x MAX44009 Illuminance sensor
//              - 1 x BH1750 Illuminance sensor
//    
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//    2018-07-02  Dan Ogorchock  Revised to demonstrate I2C sensors
//    2018-07-04  Dan Ogorchock  Added MAX44009 and BH1750 Lux Sensors
//
//******************************************************************************************
//******************************************************************************************
// SmartThings Library for ESP8266WiFi
//******************************************************************************************
#include <SmartThingsESP8266WiFi.h>

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

#include <PS_Illuminance.h>  //Implements a Polling Sensor (PS) to measure light levels via a photo resistor
#include <PS_TemperatureHumidity.h>  //Implements a Polling Sensor (PS) to measure Temperature and Humidity via DHT library
#include <PS_DS18B20_Temperature.h>  //Implements a Polling Sesnor (PS) to measure Temperature via DS18B20 libraries 
#include <PS_Water.h>        //Implements a Polling Sensor (PS) to measure presence of water (i.e. leak detector)
#include <IS_Motion.h>       //Implements an Interrupt Sensor (IS) to detect motion via a PIR sensor
#include <IS_Contact.h>      //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin
#include <IS_Smoke.h>        //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin
#include <IS_DoorControl.h>  //Implements an Interrupt Sensor (IS) and Executor to monitor the status of a digital input pin and control a digital output pin
#include <IS_Button.h>       //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin for button presses
#include <EX_Switch.h>       //Implements an Executor (EX) via a digital output to a relay
#include <EX_Alarm.h>        //Implements Executor (EX)as an Alarm Siren capability via a digital output to a relay
#include <S_TimedRelay.h>    //Implements a Sensor to control a digital output pin with timing capabilities

#include <PS_AdafruitBME280_TempHumidPress.h> //Implements a Polling Sensor (PS) to measure Temperature, humidity, and Pressure using BME280 via I2C
#include <PS_AdafruitBMP280_TempPress.h>      //Implements a Polling Sensor (PS) to measure Temperature and Pressure using BMP280 via I2C
#include <PS_AdafruitAM2320_TempHumid.h>      //Implements a Polling Sensor (PS) to measure Temperature and humidity using AM2320 via I2C
#include <PS_AdafruitTCS34725_Illum_Color.h>  //Implements a Polling Sensor (PS) to measure Color Illuminance using TCS34725 via I2C
#include <PS_AdafruitTSL2561_Illuminance.h>   //Implements a Polling Sensor (PS) to measure Illuminance using TSL2561 via I2C
#include <PS_MAX44009_Illuminance.h>          //Implements a Polling Sensor (PS) to measure Illuminance using MAX44009 via I2C
#include <PS_BH1750_Illuminance.h>            //Implements a Polling Sensor (PS) to measure Illuminance using BH1750 via I2C

//*************************************************************************************************
//NodeMCU v1.0 ESP8266-12e Pin Definitions (makes it much easier as these match the board markings)
//*************************************************************************************************
//#define LED_BUILTIN 16
//#define BUILTIN_LED 16
//
//#define D0 16  //no internal pullup resistor
//#define D1  5
//#define D2  4
//#define D3  0  //must not be pulled low during power on/reset, toggles value during boot
//#define D4  2  //must not be pulled low during power on/reset, toggles value during boot
//#define D5 14
//#define D6 12
//#define D7 13
//#define D8 15  //must not be pulled high during power on/reset

//******************************************************************************************
//Define which Arduino Pins will be used for each device
//******************************************************************************************

#define PIN_SDA D3  //works well on NodeMCU v1.0 board
#define PIN_SCL D4  //works well on NodeMCU v1.0 board

//******************************************************************************************
//ESP8266 WiFi Information
//******************************************************************************************
String str_ssid     = "yourSSIDhere";                           //  <---You must edit this line!
String str_password = "yourWiFiPasswordhere";                   //  <---You must edit this line!
IPAddress ip(192, 168, 1, 227);       //Device IP Address       //  <---You must edit this line!
IPAddress gateway(192, 168, 1, 1);    //Router gateway          //  <---You must edit this line!
IPAddress subnet(255, 255, 255, 0);   //LAN subnet mask         //  <---You must edit this line!
IPAddress dnsserver(192, 168, 1, 1);  //DNS server              //  <---You must edit this line!
const unsigned int serverPort = 8090; // port to run the http server on

// Smartthings / Hubitat Hub TCP/IP Address
IPAddress hubIp(192, 168, 1, 149);    // smartthings/hubitat hub ip //  <---You must edit this line!

// SmartThings / Hubitat Hub TCP/IP Address: UNCOMMENT line that corresponds to your hub, COMMENT the other
const unsigned int hubPort = 39500;   // smartthings hub port
//const unsigned int hubPort = 39501;   // hubitat hub port
//******************************************************************************************
//st::Everything::callOnMsgSend() optional callback routine.  This is a sniffer to monitor 
//    data being sent to ST.  This allows a user to act on data changes locally within the 
//    Arduino sktech.
//******************************************************************************************
void callback(const String &msg)
{
//  Serial.print(F("ST_Anything Callback: Sniffed data = "));
//  Serial.println(msg);
  
  //TODO:  Add local logic here to take action when a device's value/state is changed
  
  //Masquerade as the ThingShield to send data to the Arduino, as if from the ST Cloud (uncomment and edit following line)
  //st::receiveSmartString("Put your command here!");  //use same strings that the Device Handler would send
}

//******************************************************************************************
//Arduino Setup() routine
//******************************************************************************************
void setup()
{
  //******************************************************************************************
  //Declare each Device that is attached to the Arduino
  //  Notes: - For each device, there is typically a corresponding "tile" defined in your 
  //           SmartThings Device Hanlder Groovy code, except when using new COMPOSITE Device Handler
  //         - For documentation on each device's constructor arguments below, please refer to the 
  //           corresponding header (.h) and program (.cpp) files.
  //         - The name assigned to each device (1st argument below) must match the Groovy
  //           Device Handler names.  (Note: below are exceptions to this rule as some sensors
  //           produce multiple data values (e.g. "temperature1" and "humidity2".  Data from those
  //           sensors is sent to the ST Hub in multiple updates, one for each value. 
  //         - The new Composite Device Handler is comprised of a Parent DH and various Child
  //           DH's.  The names used below MUST not be changed for the Automatic Creation of
  //           child devices to work properly.  Simply increment the number by +1 for each duplicate
  //           device (e.g. contact1, contact2, contact3, etc...)  You can rename the Child Devices
  //           to match your specific use case in the ST Phone Application.
  //******************************************************************************************

  //Edit values above for SDA and SCL pins for ESP8266/ESP32 platforms.  Arduino UNO and MEGA are hardwired to specific pins.
  Wire.begin(PIN_SDA, PIN_SCL); 

  //Polling Sensors (eaxmples of various I2C sensors supported in ST_Anything)
  static st::PS_AdafruitBME280_TempHumidPress sensor1(F("BME280_1"), 60, 0, "temperature1", "humidity1", "pressure1", false, 100, 0x77);  //both BME280 and BMP280 use address 0x77(If no values are seen try 0x76 as some of the devices use this address) - only use one at a time
//  static st::PS_AdafruitBMP280_TempPress sensor2(F("BMP280_1"), 60, 10, "temperature2", "pressure2", false, 100, 0x77);  //both BME280 and BMP280 use address 0x77 - only use one at a time
  static st::PS_AdafruitAM2320_TempHumid sensor3(F("AM2320_1"), 60, 20, "temperature3", "humidity3", false, 100);  
  static st::PS_AdafruitTCS34725_Illum_Color sensor4(F("illuminancergb1"), 60, 30, TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);
  static st::PS_AdafruitTSL2561_Illuminance sensor5(F("illuminance1"), 60, 40, TSL2561_ADDR_FLOAT, TSL2561_INTEGRATIONTIME_13MS, TSL2561_GAIN_1X); 
  static st::PS_MAX44009_Illuminance sensor6(F("illuminance2"), 60, 50, MAX44009_A0_LOW); 
  static st::PS_BH1750_Illuminance sensor7(F("illuminance3"), 60, 55, BH1750_ADDR_LOW); 

  
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

  //Initialize the optional local callback routine (safe to comment out if not desired)
  st::Everything::callOnMsgSend = callback;
  
  //Create the SmartThings ESP8266WiFi Communications Object
    //STATIC IP Assignment - Recommended
    st::Everything::SmartThing = new st::SmartThingsESP8266WiFi(str_ssid, str_password, ip, gateway, subnet, dnsserver, serverPort, hubIp, hubPort, st::receiveSmartString);
 
    //DHCP IP Assigment - Must set your router's DHCP server to provice a static IP address for this device's MAC address
    //st::Everything::SmartThing = new st::SmartThingsESP8266WiFi(str_ssid, str_password, serverPort, hubIp, hubPort, st::receiveSmartString);

  //Run the Everything class' init() routine which establishes WiFi communications with SmartThings Hub
  st::Everything::init();
  
  //*****************************************************************************
  //Add each sensor to the "Everything" Class
  //*****************************************************************************
  st::Everything::addSensor(&sensor1);  //if uncommented, must comment out sensor2 below as they both use same I2C address by default
//  st::Everything::addSensor(&sensor2);  //if uncommented, must comment out sensor1 above as they both use same I2C address by default
  st::Everything::addSensor(&sensor3);
  st::Everything::addSensor(&sensor4); 
  st::Everything::addSensor(&sensor5); 
  st::Everything::addSensor(&sensor6); 
  st::Everything::addSensor(&sensor7);
             
  //*****************************************************************************
  //Add each executor to the "Everything" Class
  //*****************************************************************************
//  st::Everything::addExecutor(&executor1);
//  st::Everything::addExecutor(&executor2);
    
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
