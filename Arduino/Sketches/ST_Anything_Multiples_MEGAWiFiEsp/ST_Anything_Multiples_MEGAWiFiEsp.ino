//******************************************************************************************
//  File: ST_Anything_Multiples_MEGAWiFiEsp.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one Arduino MEGA + ESP-01 board to 
//            implement a multi input/output custom device for integration into SmartThings.
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the ESP-01 board.
//
//            ST_Anything_Multiples implements the following ST Capabilities in multiples of 2 as a demo of what is possible with a single Arduino
//              - 2 x Door Control devices (used typically for Garage Doors - input pin (contact sensor) and output pin (relay switch)
//              - 2 x Contact Sensor devices (used to monitor magnetic door sensors)
//              - 2 x Switch devices (used to turn on a digital output (e.g. LED, relay, etc...)
//              - 2 x Water Sensor devices (using an analog input pin to measure voltage from a water detector baord)
//              - 2 x Illuminance Measurement devices (using a photoresitor attached to ananlog input)
//              - 2 x Voltage Measurement devices (using a photoresitor attached to ananlog input)
//              - 2 x Smoke Detector devices (using simple digital input)
//              - 2 x Carbon Monoxide Detector devices (using simple digital input)
//              - 2 x Motion devices (used to detect motion)
//              - 2 x Temperature Measurement devices (Temperature from DHT22 device)
//              - 2 x Humidity Measurement devices (Humidity from DHT22 device)
//              - 2 x Relay Switch devices (used to turn on a digital output for a set number of cycles And On/Off times (e.g.relay, etc...))
//              - 2 x Button devices (sends "pushed" if held for less than 1 second, else sends "held"
//              - 2 x Alarm devices - 1 siren only, 1 siren and strobe (using simple digital outputs)
//
//            This example requires the use of an Arduino MEGA2560 since the ESP-01 WiFi board
//            needs a high-speed UART for communications.  This example assumes the ESP-01 is
//            connected to the MEGA's "Serial1" UART on pins 18 & 19.
//
//    
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//    2017-02-12  Dan Ogorchock  Revised to use the new SMartThings v2.0 library
//    2017-02-20  Dan Ogorchock  Revised to use the ESP-01 board
//    2017-04-16  Dan Ogorchock  New sketch to demonstrate multiple SmartThings Capabilties of each type
//    2017-04-22  Dan Ogorchock  Added Voltage, Carbon Monoxide, and Alarm with Strobe
//    2018-02-09  Dan Ogorchock  Added support for Hubitat Elevation Hub
//    2019-07-01  Dan.t		 Added support for websocket Logging, st::debugPrint and st::debugPrintln
//
//******************************************************************************************
//******************************************************************************************
// SmartThings Library for Arduino + ESP-01 board combination.
//******************************************************************************************
#include <SmartThingsWiFiEsp.h>    //Library to provide API to the SmartThings ESP-01 WiFi board

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

#include <PS_Illuminance.h>  //Implements a Polling Sensor (PS) to measure light levels via a photo resistor on an analog input pin 
#include <PS_Voltage.h>      //Implements a Polling Sensor (PS) to measure voltage on an analog input pin 
#include <PS_TemperatureHumidity.h>  //Implements a Polling Sensor (PS) to measure Temperature and Humidity via DHT library
#include <PS_Water.h>        //Implements a Polling Sensor (PS) to measure presence of water (i.e. leak detector) on an analog input pin 
#include <IS_Motion.h>       //Implements an Interrupt Sensor (IS) to detect motion via a PIR sensor on a digital input pin
#include <IS_Contact.h>      //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin
#include <IS_Smoke.h>        //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin
#include <IS_CarbonMonoxide.h> //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin
#include <IS_DoorControl.h>  //Implements an Interrupt Sensor (IS) and Executor to monitor the status of a digital input pin and control a digital output pin
#include <IS_Button.h>       //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin for button presses
#include <EX_Switch.h>       //Implements an Executor (EX) via a digital output to a relay
#include <EX_Alarm.h>        //Implements Executor (EX)as an Alarm capability with Siren and Strobe via digital outputs to relays
#include <S_TimedRelay.h>    //Implements a Sensor to control a digital output pin with timing/cycle repeat capabilities

//**********************************************************************************************************
//Define which Arduino Pins will be used for each device
//  Notes: Arduino communicates with the ESP-01 using the "Serial1" Hardware Serial UART. 
//         This is on digital pins 18 & 19 on the Mega. 
//**********************************************************************************************************
//"RESERVED" pins for ESP-01 - best to avoid
#define PIN_18_RESERVED          18  //reserved ESP-01 on MEGA
#define PIN_19_RESERVED          19  //reserved ESP-01 on MEGA

//Analog Pins
#define PIN_WATER_1               A0  //SmartThings Capability "Water Sensor"
#define PIN_WATER_2               A1  //SmartThings Capability "Water Sensor"
#define PIN_ILLUMINANCE_1         A2  //SmartThings Capability "Illuminance Measurement"
#define PIN_ILLUMINANCE_2         A3  //SmartThings Capability "Illuminance Measurement"
#define PIN_VOLTAGE_1             A4  //SmartThings Capability "Voltage Measurement"
#define PIN_VOLTAGE_2             A5  //SmartThings Capability "Voltage Measurement"

//Digital Pins
#define PIN_TEMPERATUREHUMIDITY_1 22  //SmartThings Capabilities "Temperature Measurement" and "Relative Humidity Measurement"
#define PIN_TEMPERATUREHUMIDITY_2 23  //SmartThings Capabilities "Temperature Measurement" and "Relative Humidity Measurement"
#define PIN_MOTION_1              24  //SmartThings Capability "Motion Sensor"
#define PIN_MOTION_2              25  //SmartThings Capability "Motion Sensor"
#define PIN_CONTACT_1             26  //SmartThings Capability "Contact Sensor"
#define PIN_CONTACT_2             27  //SmartThings Capability "Contact Sensor"
#define PIN_SWITCH_1              28  //SmartThings Capability "Switch"
#define PIN_SWITCH_2              29  //SmartThings Capability "Switch"
#define PIN_TIMEDRELAY_1          30  //SmartThings Capability "Relay Switch"
#define PIN_TIMEDRELAY_2          31  //SmartThings Capability "Relay Switch"
#define PIN_SMOKE_1               32  //SmartThings Capability "Smoke Detector"
#define PIN_SMOKE_2               33  //SmartThings Capability "Smoke Detector"
#define PIN_ALARM_1               34  //SmartThings Capability "Alarm"
#define PIN_ALARM_2               40  //SmartThings Capability "Alarm"
#define PIN_STROBE_2              41  //SmartThings Capability "Alarm"              
#define PIN_CO_1                  42  //SmartThings Capability "Carbon Monoxide Detector"
#define PIN_CO_2                  43  //SmartThings Capability "Carbon Monoxide Detector"

//Garage Door Pins 
#define PIN_DOORCONTROL_CONTACT_1 35  //SmartThings Capabilty "Door Control" 
#define PIN_DOORCONTROL_RELAY_1   36  //SmartThings Capabilty "Door Control" 
#define PIN_DOORCONTROL_CONTACT_2 37  //SmartThings Capabilty "Door Control"  
#define PIN_DOORCONTROL_RELAY_2   38  //SmartThings Capabilty "Door Control" 

//Pushbutton Pins
#define PIN_BUTTON1               48  //SmartThings Capabilty Button / Holdable Button
#define PIN_BUTTON2               49  //SmartThings Capabilty Button / Holdable Button

//******************************************************************************************
//WiFiEsp Information
//****************************************************************************************** 
String str_ssid     = "yourSSIDhere";                            //  <---You must edit this line!
String str_password = "yourWiFiPasswordhere";                    //  <---You must edit this line!
IPAddress ip(192, 168, 1, 228);       // Device IP Address       //  <---You must edit this line!
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
  //Uncomment if it weould be desirable to using this function
  //st::debugPrint(F("ST_Anything_Miltiples Callback: Sniffed data = "));
  //st::debugPrintln(msg);
  
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
  static st::PS_Water               sensor1(F("water1"), 60, 0, PIN_WATER_1, 200);
  static st::PS_Water               sensor2(F("water2"), 60, 10, PIN_WATER_2, 200);
  static st::PS_Illuminance         sensor3(F("illuminance1"), 60, 20, PIN_ILLUMINANCE_1, 0, 1023, 0, 1000);
  static st::PS_Illuminance         sensor4(F("illuminance2"), 60, 30, PIN_ILLUMINANCE_2, 0, 1023, 0, 1000);
  static st::PS_TemperatureHumidity sensor5(F("temphumid1"), 60, 40, PIN_TEMPERATUREHUMIDITY_1, st::PS_TemperatureHumidity::DHT22,"temperature1","humidity1");
  static st::PS_TemperatureHumidity sensor6(F("temphumid2"), 60, 50, PIN_TEMPERATUREHUMIDITY_2, st::PS_TemperatureHumidity::DHT22,"temperature2","humidity2");
  static st::PS_Voltage             sensor7(F("voltage1"), 60, 55, PIN_VOLTAGE_1, 0, 1023, 0, 5000);
  static st::PS_Voltage             sensor8(F("voltage2"), 60, 57, PIN_VOLTAGE_2, 0, 1023, 0, 5000);
  
  //Interrupt Sensors 
  static st::IS_Motion              sensor9(F("motion1"), PIN_MOTION_1, HIGH, false, 500);
  static st::IS_Motion              sensor10(F("motion2"), PIN_MOTION_2, HIGH, false, 500);
  static st::IS_Contact             sensor11(F("contact1"), PIN_CONTACT_1, LOW, true, 500);
  static st::IS_Contact             sensor12(F("contact2"), PIN_CONTACT_2, LOW, true, 500);
  static st::IS_Smoke               sensor13(F("smoke1"), PIN_SMOKE_1, HIGH, true, 500);
  static st::IS_Smoke               sensor14(F("smoke2"), PIN_SMOKE_2, HIGH, true, 500);
  static st::IS_DoorControl         sensor15(F("doorControl1"), PIN_DOORCONTROL_CONTACT_1, LOW, true, PIN_DOORCONTROL_RELAY_1, LOW, true, 1000);
  static st::IS_DoorControl         sensor16(F("doorControl2"), PIN_DOORCONTROL_CONTACT_2, LOW, true, PIN_DOORCONTROL_RELAY_2, LOW, true, 1000);
  static st::IS_Button              sensor17(F("button1"), PIN_BUTTON1, 1000, LOW, true, 500);
  static st::IS_Button              sensor18(F("button2"), PIN_BUTTON2, 1000, LOW, true, 500);
  static st::IS_CarbonMonoxide      sensor19(F("carbonMonoxide1"), PIN_CO_1, HIGH, true, 500);
  static st::IS_CarbonMonoxide      sensor20(F("carbonMonoxide2"), PIN_CO_2, HIGH, true, 500);

  //Special sensors/executors (uses portions of both polling and executor classes)
  static st::S_TimedRelay           sensor21(F("relaySwitch1"), PIN_TIMEDRELAY_1, LOW, true, 3000, 0, 1);
  static st::S_TimedRelay           sensor22(F("relaySwitch2"), PIN_TIMEDRELAY_2, LOW, true, 3000, 0, 1);

  //Executors
  static st::EX_Switch              executor1(F("switch1"), PIN_SWITCH_1, LOW, true);
  static st::EX_Switch              executor2(F("switch2"), PIN_SWITCH_2, LOW, true);
  static st::EX_Alarm               executor3(F("alarm1"), PIN_ALARM_1, LOW, true);
  static st::EX_Alarm               executor4(F("alarm2"), PIN_ALARM_2, LOW, true, PIN_STROBE_2);
   
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
  
  //Create the SmartThings WiFiEsp (ESP-01) Communications Object
    //STATIC IP Assignment - Recommended
    st::Everything::SmartThing = new st::SmartThingsWiFiEsp(&Serial1, str_ssid, str_password, ip, serverPort, hubIp, hubPort, st::receiveSmartString);
 
    //DHCP IP Assigment - Must set your router's DHCP server to provice a static IP address for this device's MAC address
    //st::Everything::SmartThing = new st::SmartThingsWiFiEsp(&Serial1, str_ssid, str_password, serverPort, hubIp, hubPort, st::receiveSmartString);

  //Initialize the Serial1 baudrate to match your ESP-01's baud rate (e.g. 9600, 57600, 115200)
  Serial1.begin(115200); 
  
  //Run the Everything class' init() routine which establishes Ethernet communications with the SmartThings Hub
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
  st::Everything::addSensor(&sensor16); 
  st::Everything::addSensor(&sensor17); 
  st::Everything::addSensor(&sensor18); 
  st::Everything::addSensor(&sensor19); 
  st::Everything::addSensor(&sensor20); 
  st::Everything::addSensor(&sensor21); 
  st::Everything::addSensor(&sensor22); 
    
  //*****************************************************************************
  //Add each executor to the "Everything" Class
  //*****************************************************************************
  st::Everything::addExecutor(&executor1);
  st::Everything::addExecutor(&executor2);
  st::Everything::addExecutor(&executor3);
  st::Everything::addExecutor(&executor4);
  
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
