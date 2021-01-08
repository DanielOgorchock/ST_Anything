//******************************************************************************************
//  File: ST_Anything_Multiples_ESP8266WiFi.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one ESP8266 to 
//            implement a multi input/output custom device for integration into SmartThings.
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the NodeMCU ESP8266's WiFi.
//
//            ST_Anything_Multiples implements the following ST Capabilities as a demo of what is possible with a single NodeMCU ESP8266
//              - 1 x Ultrasonic device (sends distance value to ST, and its value is pushed to water sensor)
//              - 1 x Water Sensor device (uses the distance sensor value to determine dry/wet condition)
//    
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2020-01-08  Andy A.        Initial version, copied from 'ST_Anything_Multiples_ESP8266WiFi.ino'
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

#include <PS_Ultrasonic.h>   //Ultrasonic Distance Measurement Sensor, being used to monitor water level in cylindrical tank
#include <PS_Water.h>        //Water sensor, but we will link it to the ultrasonic sensor rather than use its own analog inputs for data

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
#define PIN_ULTRASONIC_T 2  // digital output to trigger ultrasonic
#define PIN_ULTRASONIC_E 0  // digital input to read the echo

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

// Smarthings Hub Information
//IPAddress hubIp(192, 168, 1, 149);  // smartthings hub ip       //  <---You must edit this line!
//const unsigned int hubPort = 39500; // smartthings hub port

// Hubitat Hub Information
IPAddress hubIp(192, 168, 1, 143);    // hubitat hub ip         //  <---You must edit this line!
const unsigned int hubPort = 39501;   // hubitat hub port

String callbackData;

//******************************************************************************************
// st::Everything::callOnMsgSend() optional callback routine.  This is a sniffer
// to monitor
//    data being sent to ST.  This allows a user to act on data changes locally
//    within the Arduino sktech.
//******************************************************************************************
void callback(const String& msg) 
{
    Serial.print(F("ST_Anything Callback: Sniffed data = "));
    Serial.println(msg);

    // Check if ultrasonic1 is posting an update to SmartThings
    if (msg.startsWith("ultrasonic1")) 
    {
        // global variable, reserved buffer
        callbackData = msg.substring(msg.indexOf(' '));
        callbackData.trim();

        Serial.print(F("Ultrasonic value: "));
        Serial.println(callbackData);

        if (callbackData.length() > 0)
        {
            // data maybe a float, but PS_Water expects an integer
            int sensorValue = (int)round(callbackData.toFloat());
            st::Device *device = st::Everything::getDeviceByName("water1");
            if (device != NULL) 
            {
                Serial.print("Setting water device to value ");
                Serial.println(sensorValue);
                st::PS_Water *water = (st::PS_Water *)device;
                water->setSensorValue(sensorValue);
            }
            else 
            {
                Serial.println(F("Unable to find device 'water1'"));
            }

            callbackData.remove(0);
        }
        else
        {
            Serial.println("No sensor value available");
        }
        
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
  //           SmartThings Device Hanlder Groovy code, except when using new COMPOSITE Device Handler
  //         - For details on each device's constructor arguments below, please refer to the 
  //           corresponding header (.h) and program (.cpp) files.
  //         - The name assigned to each device (1st argument below) must match the Groovy
  //           Device Handler names.  (Note: "temphumid" below is the exception to this rule
  //           as the DHT sensors produce both "temperature" and "humidity".  Data from that
  //           particular sensor is sent to the ST Hub in two separate updates, one for 
  //           "temperature" and one for "humidity")
  //         - The new Composite Device Handler is comprised of a Parent DH and various Child
  //           DH's.  The names used below MUST not be changed for the Automatic Creation of
  //           child devices to work properly.  Simply increment the number by +1 for each duplicate
  //           device (e.g. contact1, contact2, contact3, etc...)  You can rename the Child Devices
  //           to match your specific use case in the ST Phone Application.
  //******************************************************************************************
  callbackData.reserve(100);

  const unsigned int POLLING_INTERVAL_SEC = 60;
  const int WATER_LIMIT_CM = 20;

  // Polling Sensors
  static st::PS_Ultrasonic sensor1(F("ultrasonic1"), POLLING_INTERVAL_SEC, (int)random(0, 59), PIN_ULTRASONIC_T, PIN_ULTRASONIC_E);
  static st::PS_Water sensor2(F("water1"), POLLING_INTERVAL_SEC, (int)random(0, 59), WATER_LIMIT_CM, true);

  //Special sensors/executors (uses portions of both polling and executor classes)
  
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
  st::Everything::addSensor(&sensor1);
  st::Everything::addSensor(&sensor2);
      
  //*****************************************************************************
  //Add each executor to the "Everything" Class
  //*****************************************************************************
    
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
