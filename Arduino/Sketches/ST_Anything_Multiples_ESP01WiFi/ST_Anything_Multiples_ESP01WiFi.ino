//******************************************************************************************
//  File: ST_Anything_Multiples_ESP01WiFi.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one ESP8266-01 (ESP-01) to 
//            implement a multi input/output custom device for integration into SmartThings.
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the ESP-01's WiFi.
//    
//            ST_Anything_Multiples implements the following ST Capabilities in multiples of 1 as a demo of what is possible with a single ESP-01
//              - 1 x Contact Sensor device (used to monitor magnetic door sensors)
//              - 1 x Motion devices (used to detect motion)
//
//  Note:  The tiny ESP-01 only has 2 GPIO pins, so this example is somewhat limited.  Use the ST_ANything_Multiples_ESP8266WiFi.ino example to see 
//         what else is possible.  As long as you only try using 2 pins, you can use them for whatever you'd like.
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//    2017-02-12  Dan Ogorchock  Revised to use the new SmartThings v2.0 library
//    2017-02-21  Dan Ogorchock  New example specifically for running everythin on a ESP-01 (no Arduino required!)
//    2017-04-24  Dan Ogorchock  Updated for use with new v2.5 Parent/Child Device handlers
//    2017-12-27  Rodney Yates    Adds support for OTA updating through ArduinoOTA and/or Web
//
//******************************************************************************************
//******************************************************************************************
// SmartThings Library for ESP8266WiFi
//******************************************************************************************
#include <SmartThingsESP8266WiFi.h>

//******************************************************************************************
// ArduinoOTA Library for ESP8266WiFi
//******************************************************************************************
//   Configuration
#define ArdOTA  // Enables ArduinoOTA, uncomment to enable
#define WebOTA  // Enables web updating, uncomment to enable

#ifdef ArdOTA
  #include <ESP8266mDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
#endif
#ifdef WebOTA
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266mDNS.h>
  #include <ESP8266HTTPUpdateServer.h>
  ESP8266WebServer httpServer(80);
  ESP8266HTTPUpdateServer httpUpdater;
#endif

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

#include <PS_TemperatureHumidity.h>  //Implements a Polling Sensor (PS) to measure Temperature and Humidity via DHT library. 
#include <PS_Water.h>        //Implements a Polling Sensor (PS) to measure presence of water (i.e. leak detector)
#include <IS_Motion.h>       //Implements an Interrupt Sensor (IS) to detect motion via a PIR sensor
#include <IS_Contact.h>      //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin
#include <EX_Switch.h>       //Implements an Executor (EX) via a digital output to a relay
#include <EX_Alarm.h>        //Implements Executor (EX)as an Alarm Siren capability via a digital output to a relay

//******************************************************************************************
//Define which Arduino Pins will be used for each device
//******************************************************************************************

#define PIN_MOTION               0
#define PIN_CONTACT              2

//******************************************************************************************
//ESP8266 WiFi Information
//******************************************************************************************
String str_ssid     = "yourSSIDhere";                           //  <---You must edit this line!
String str_password = "yourWiFiPasswordhere";                   //  <---You must edit this line!
const char* deviceName = "yourDeviceName";                      //  <---You must edit this line!
WiFi.hostname(deviceName);
IPAddress ip(192, 168, 1, 230);       //Device IP Address       //  <---You must edit this line!
IPAddress gateway(192, 168, 1, 1);    //Router gateway          //  <---You must edit this line!
IPAddress subnet(255, 255, 255, 0);   //LAN subnet mask         //  <---You must edit this line!
IPAddress dnsserver(192, 168, 1, 1);  //DNS server              //  <---You must edit this line!
const unsigned int serverPort = 8090; // port to run the http server on

// Smartthings Hub Information
IPAddress hubIp(192, 168, 1, 149);    // smartthings hub ip     //  <---You must edit this line!
const unsigned int hubPort = 39500;   // smartthings hub port

//******************************************************************************************
//st::Everything::callOnMsgSend() optional callback routine.  This is a sniffer to monitor 
//    data being sent to ST.  This allows a user to act on data changes locally within the 
//    Arduino sktech.
//******************************************************************************************
void callback(const String &msg)
{
  //Serial.print(F("ST_Anything Callback: Sniffed data = "));
  //Serial.println(msg);
  
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
  //Declare each Device that is attached to the ESP-01
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
 
  //Interrupt Sensors 
  static st::IS_Motion sensor1(F("motion1"), PIN_MOTION, LOW, true);
  static st::IS_Contact sensor2(F("contact1"), PIN_CONTACT, LOW, true);
  
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

  //*****************************************************************************
  //  ArduinoOTA setup
  //*****************************************************************************
  #ifdef ArdOTA
    //ArduinoOTA.setPassword((const char *)"123");  // Set ArduinoOTA Password. No authentication by default.
    const unsigned int portOTA = 8266;                // Port default is 8266
    ArduinoOTA.setPort(portOTA);
    ArduinoOTA.setHostname(deviceName);
    
    ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
     });
    ArduinoOTA.begin();
    Serial.println("ArduinoOTA Ready!");
    Serial.println("At IP address: " + WiFi.localIP().toString());
    Serial.println("Port: " + portOTA);    
  #endif
  
  //*****************************************************************************
  //  WebpageOTA setup
  //*****************************************************************************
  #ifdef WebOTA
    
    const char* update_username = "admin";  // Set ArduinoOTA Password. No authentication by default.
    const char* update_password = "esp8266";
    const char* update_path = "/"; // update path = http://yourip/firmware
    
    MDNS.begin(deviceName);
    httpUpdater.setup(&httpServer, update_path, update_username, update_password);
    httpServer.begin();
    MDNS.addService("http", "tcp", 80);
    Serial.printf("HTTPUpdateServer ready! Open http://%s%s in your browser and login with username '%s' and password '%s'\n", WiFi.localIP().toString(), update_path, update_username, update_password);
  #endif
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
  
  httpServer.handleClient(); //ESP8266 web updater
}
