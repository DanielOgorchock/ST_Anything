//******************************************************************************************
//  File: ST_Anything_MKR_THERM.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one Arduino MKR1010 to implement a multiple
//            input/output custom device for integration into SmartThings.
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the WiFiNINA onboard module.
//
//            ST_Anything_Multiples implements the following ST Capabilities in multiples of 2 as a demo of what is possible with a single Arduino
//              - 1 x Temperature Measurement devices (Temperature from MKR THERM shield)
//
//            This example requires the use of an Arduino MKR1010.
// 
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2019-06-23  Dan Ogorchock  New example sketch for use with the Arduino MKR1010
//
//******************************************************************************************
//******************************************************************************************
// SmartThings Library for Arduino MKR1010.
//******************************************************************************************
#include <SmartThingsWiFiNINA.h>    //Library to provide API to the SmartThings WiFi NINA Module

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

#include <PS_MKR_THERM.h>    //Implements a Polling Sensor (PS) to measure Temperature via Arduino MKR THERM library

//**********************************************************************************************************
//Define which Arduino Pins will be used for each device
//  Notes: Arduino MKR1010 communicates directly to the onboard WiFi NINA module without interfering 
//         with remaining MKR1010 pins.  Thus A0-A6 and D0-D14 an be used for your sketch. 
//**********************************************************************************************************

//Analog Pins

//Digital Pins

//******************************************************************************************
//WiFiNINA Information
//****************************************************************************************** 
String str_ssid     = "yourSSIDhere";                            //  <---You must edit this line!
String str_password = "yourWiFiPasswordhere";                    //  <---You must edit this line!
IPAddress ip(192, 168, 1, 234);       // Device IP Address       //  <---You must edit this line!
IPAddress gateway(192, 168, 1, 1);    //Router gateway           //  <---You must edit this line!
IPAddress subnet(255, 255, 255, 0);   //LAN subnet mask          //  <---You must edit this line!
IPAddress dnsserver(192, 168, 1, 1);  //DNS server               //  <---You must edit this line!
const unsigned int serverPort = 8090; // port to run the http server on

// Smartthings Hub Information
//IPAddress hubIp(192, 168, 1, 149);    // smartthings hub ip     //  <---You must edit this line!
//const unsigned int hubPort = 39500;   // smartthings hub port

// Hubitat Hub Information
IPAddress hubIp(192, 168, 1, 145);    // Hubitat hub ip     //  <---You must edit this line!
const unsigned int hubPort = 39501;   // Hubitat hub port


//******************************************************************************************
//st::Everything::callOnMsgSend() optional callback routine.  This is a sniffer to monitor 
//    data being sent to ST.  This allows a user to act on data changes locally within the 
//    Arduino sktech.
//******************************************************************************************
void callback(const String &msg)
{
  //Uncomment if it weould be desirable to using this function
  //Serial.print(F("ST_Anything_Miltiples Callback: Sniffed data = "));
  //Serial.println(msg);
  
  //TODO:  Add local logic here to take action when a device's value/state is changed
  
  //Masquerade as the ThingShield to send data to the Arduino, as if from the ST Cloud (uncomment and edit following line(s) as you see fit)
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
//Polling Sensors 
  static st::PS_MKR_THERM           sensor1(F("temperature1"), 30, 0); 
  
  //Interrupt Sensors 

  //Special sensors/executors (uses portions of both polling and executor classes)

  //Executors
   
  //*****************************************************************************
  //  Configure debug print output from each main class 
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
  
  //Create the SmartThings WiFiNINA Communications Object
    //STATIC IP Assignment - Recommended
    st::Everything::SmartThing = new st::SmartThingsWiFiNINA(str_ssid, str_password, ip, gateway, subnet, dnsserver, serverPort, hubIp, hubPort, st::receiveSmartString);
 
    //DHCP IP Assigment - Must set your router's DHCP server to provice a static IP address for this device's MAC address
    //st::Everything::SmartThing = new st::SmartThingsWiFiNINA(str_ssid, str_password, serverPort, hubIp, hubPort, st::receiveSmartString);
  
  //Run the Everything class' init() routine which establishes Ethernet communications with the SmartThings Hub
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
