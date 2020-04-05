//******************************************************************************************
//  File: ST_Anything_MCP23008_ESP8266WiFi.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one NodeMCU ESP8266 to 
//            implement a multi input/output custom device for integration into SmartThings/Hubitat.
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the NodeMCU ESP8266's WiFi.
//
//            ST_Anything_MCP23008 implements the following ST Capabilities on a NodeMCU ESP8266
//            8 - MCP23008 attached Timed Relay Switches
//    
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2020-01-16  Doug (M2)    Converted the I2C file to MCP23008  
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

#include <Adafruit_MCP23008.h>
#include <S_TimedRelay_MCP.h>    //Implements a Sensor to control a digital output pin with timing capabilities
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

#define PIN_SDA D3  //To Pin 2 on MCP23008
#define PIN_SCL D4  //To Pin 1 on MCP23008

#define PIN_TIMEDRELAY_0 0 //MCP23008 Pin 10
#define PIN_TIMEDRELAY_1 1 //MCP23008 Pin 11
#define PIN_TIMEDRELAY_2 2 //MCP23008 Pin 12
#define PIN_TIMEDRELAY_3 3 //MCP23008 Pin 13
#define PIN_TIMEDRELAY_4 4 //MCP23008 Pin 14
#define PIN_TIMEDRELAY_5 5 //MCP23008 Pin 15
#define PIN_TIMEDRELAY_6 6 //MCP23008 Pin 16
#define PIN_TIMEDRELAY_7 7 //MCP23008 Pin 17


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
IPAddress hubIp(192, 168, 1, 145);    // hubitat hub ip         //  <---You must edit this line!
const unsigned int hubPort = 39501;   // hubitat hub port

//******************************************************************************************
//st::Everything::callOnMsgSend() optional callback routine.  This is a sniffer to monitor 
//    data being sent to ST.  This allows a user to act on data changes locally within the 
//    Arduino sketch.
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
  //           SmartThings Device Handler Groovy code, except when using new COMPOSITE Device Handler
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
  // use default address 0 (A0, A1 and A2 on MCP all grounded). Pass the decimal address if other than 0.
  
  Adafruit_MCP23008 mcp;
  mcp.begin();    //e.g.  IF A0 and A1 = VCC, A2 = Gnd. then use mcp.begin(3);  
  mcp.writeGPIO(0xFF);  // force the register HIGH on all pins so the active low relays are not triggered when the pinMode sets to OUTPUT
  // mcp.pinMode(0, OUTPUT);
  // mcp.pinMode(1, OUTPUT);
  // mcp.pinMode(2, OUTPUT);
  // mcp.pinMode(3, OUTPUT);
  // mcp.pinMode(4, OUTPUT);
  // mcp.pinMode(5, OUTPUT);
  // mcp.pinMode(6, OUTPUT);
  // mcp.pinMode(7, OUTPUT);

  
  //Special sensors/executors (uses portions of both polling and executor classes)
  static st::S_TimedRelay_MCP sensor1(F("relaySwitch1"), PIN_TIMEDRELAY_0, LOW, true, 700, 0, 1);
  static st::S_TimedRelay_MCP sensor2(F("relaySwitch2"), PIN_TIMEDRELAY_1, LOW, true, 700, 0, 1);
  static st::S_TimedRelay_MCP sensor3(F("relaySwitch3"), PIN_TIMEDRELAY_2, LOW, true, 700, 0, 1);
  static st::S_TimedRelay_MCP sensor4(F("relaySwitch4"), PIN_TIMEDRELAY_3, LOW, true, 700, 0, 1);
  static st::S_TimedRelay_MCP sensor5(F("relaySwitch5"), PIN_TIMEDRELAY_4, LOW, true, 700, 0, 1);
  static st::S_TimedRelay_MCP sensor6(F("relaySwitch6"), PIN_TIMEDRELAY_5, LOW, true, 700, 0, 1);
  static st::S_TimedRelay_MCP sensor7(F("relaySwitch7"), PIN_TIMEDRELAY_6, LOW, true, 700, 0, 1);
  static st::S_TimedRelay_MCP sensor8(F("relaySwitch8"), PIN_TIMEDRELAY_7, LOW, true, 700, 0, 1);


  
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
 
    //DHCP IP Assignment - Must set your router's DHCP server to provide a static IP address for this device's MAC address
    //st::Everything::SmartThing = new st::SmartThingsESP8266WiFi(str_ssid, str_password, serverPort, hubIp, hubPort, st::receiveSmartString);

  //Run the Everything class' init() routine which establishes WiFi communications with SmartThings Hub
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
