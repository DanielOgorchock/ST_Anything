//******************************************************************************************
//  File: ST_Anything_WiFiEsp.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one Arduino + ESP-01 board to 
//            implement a multi input/output custom device for integration into SmartThings.
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the ESP-01 board.
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

#include <PS_Illuminance.h>  //Implements a Polling Sensor (PS) to measure light levels via a photo resistor

#include <PS_TemperatureHumidity.h>  //Implements a Polling Sensor (PS) to measure Temperature and Humidity via DHT library
#include <PS_Water.h>        //Implements a Polling Sensor (PS) to measure presence of water (i.e. leak detector)
#include <IS_Motion.h>       //Implements an Interrupt Sensor (IS) to detect motion via a PIR sensor
#include <IS_Contact.h>      //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin
#include <EX_Switch.h>       //Implements an Executor (EX) via a digital output to a relay
#include <EX_Alarm.h>        //Implements Executor (EX)as an Alarm Siren capability via a digital output to a relay

//**********************************************************************************************************
//Define which Arduino Pins will be used for each device
//  Notes: Arduino communicates with the ESP-01 using the "Serial1" Hardware Serial UART. 
//         This is on digital pins 18 & 19 on the Mega. 
//**********************************************************************************************************
//"RESERVED" pins for ESP-01 - best to avoid
#define PIN_18_RESERVED          18  //reserved ESP-01 on MEGA
#define PIN_19_RESERVED          19  //reserved ESP-01 on MEGA

#define PIN_WATER                A4
#define PIN_ILLUMINANCE          A5
#define PIN_TEMPERATUREHUMIDITY  5
#define PIN_MOTION               6
#define PIN_CONTACT              7
#define PIN_SWITCH               8
#define PIN_ALARM                9

//******************************************************************************************
//WiFiEsp Information
//****************************************************************************************** 
String str_ssid     = "yourSSIDhere";                            //  <---You must edit this line!
String str_password = "yourWiFiPasswordhere";                    //  <---You must edit this line!
IPAddress ip(192, 168, 1, 205);       // Device IP Address       //  <---You must edit this line!
const unsigned int serverPort = 8090; // port to run the http server on

// Smartthings hub information
IPAddress hubIp(192,168,1,149);       // smartthings hub ip      //  <---You must edit this line!
const unsigned int hubPort = 39500;   // smartthings hub port

//******************************************************************************************
//st::Everything::callOnMsgSend() optional callback routine.  This is a sniffer to monitor 
//    data being sent to ST.  This allows a user to act on data changes locally within the 
//    Arduino sktech.
//******************************************************************************************
void callback(const String &msg)
{
  Serial.print(F("ST_Anything Callback: Sniffed data = "));
  Serial.println(msg);
  
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
  //           SmartThings DeviceType Groovy code
  //         - For details on each device's constructor arguments below, please refer to the 
  //           corresponding header (.h) and program (.cpp) files.
  //         - The name assigned to each device (1st argument below) must match the Groovy
  //           DeviceType Tile name.  (Note: "temphumid" below is the exception to this rule
  //           as the DHT sensors produce both "temperature" and "humidity".  Data from that
  //           particular sensor is sent to the ST Hub in two separate updates, one for 
  //           "temperature" and one for "humidity")
  //******************************************************************************************
  //Polling Sensors
  static st::PS_Illuminance sensor1(F("illuminance"), 120, 0, PIN_ILLUMINANCE, 0, 1024, 0, 1000);
  static st::PS_TemperatureHumidity sensor2(F("temphumid"), 120, 10, PIN_TEMPERATUREHUMIDITY, st::PS_TemperatureHumidity::DHT22);
  static st::PS_Water sensor3(F("water"), 60, 20, PIN_WATER, 200);
  
  //Interrupt Sensors 
  static st::IS_Motion sensor4(F("motion"), PIN_MOTION, HIGH, false);
  static st::IS_Contact sensor5(F("contact"), PIN_CONTACT, LOW, true);
  
  //Executors
  static st::EX_Switch executor1(F("switch"), PIN_SWITCH, LOW, true);
  static st::EX_Alarm executor2(F("alarm"), PIN_ALARM, LOW, true);
  
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
  st::Everything::SmartThing = new st::SmartThingsWiFiEsp(&Serial1, str_ssid, str_password, ip, serverPort, hubIp, hubPort, st::receiveSmartString);

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
  
  //*****************************************************************************
  //Add each executor to the "Everything" Class
  //*****************************************************************************
  st::Everything::addExecutor(&executor1);
  st::Everything::addExecutor(&executor2);
  
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
