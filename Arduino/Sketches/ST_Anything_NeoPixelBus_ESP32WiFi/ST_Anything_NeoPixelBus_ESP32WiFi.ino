//******************************************************************************************
//  File: ST_Anything_NeoPixelBus_ESP32WiFi.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one ESP32 to implement 
//            a multi input/output custom device for integration into SmartThings.
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the ESP32's WiFi.
//
//            ST_Anything_NeoPixelBus_ESP32WiFi implements the following ST Capabilities as a demo of what is possible with a single ESP32
//              - 2 x RGB Addressable LED (used to set the color of a Programmable RGB LED strip)
//              - 2 x RGBW Addressable LED (used to set the color of a Programmable RGBW LED strip)
//
//    
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2017-08-14  Dan Ogorchock  Original Creation - Adapted from ESP8266 to work with ESP32 board
//    2017-10-06  Allan (vseven) Modified for RGB example
//    2017-10-13  Allan (vseven) Modified for RGBW example
//    2018-02-09  Dan Ogorchock  Added support for Hubitat Elevation Hub
//    2020-08-01  Allan (vseven) Modified for programmable LED's using the NeoPixelBus library
//    2020-08-17  Allan (vseven) Modified for programmable LED's using a templated library for multistrip control
//
//   Special thanks to Joshua Spain for his contributions in porting ST_Anything to the ESP32!
//
//******************************************************************************************
//******************************************************************************************
// SmartThings Library for ESP32WiFi
//******************************************************************************************
#include <SmartThingsESP32WiFi.h>

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

#include <EX_RGB_NeoPixelBus_T.h>    //Implements an Executor (EX) for a RGB LED strip using the NeoPixelBus library. 
#include <EX_RGBW_NeoPixelBus_T.h>   //Implements an Executor (EX) for a RGBW LED strip using the NeoPixelBus library. 

//******************************************************************************************
// ESP32 BlueTooth serial monitor support.  Uncomment to use.
// Note: Adding BT support may require a larger partition.  Do this under Tool -> Partition Scheme -> Huge APP
//       You will also need a computer with BlueTooth support or a BlueTooth app on your smart phone such 
//       as "Serial BlueTooth Terminal" (https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal&hl=en_US)
//******************************************************************************************
//#include <BluetoothSerial.h>  //Header File for Serial Bluetooth
//#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
//#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
//#endif
//BluetoothSerial SerialBT;     //Object for Bluetooth

//****************************************************************************************************************************
//NodeMCU-32s ESP32 Pin Definitions (just for reference from ..hardware\espressif\esp32\variants\nodemcu-32s\pins_arduino.h)
//****************************************************************************************************************************
//#define LED_BUILTIN 2
//#define BUILTIN_LED 2
//
//#define A0 = 36;
//#define A3 = 39;
//#define A4 = 32;
//#define A5 = 33;
//#define A6 = 34;
//#define A7 = 35;
//#define A10 = 4;
//#define A11 = 0;
//#define A12 = 2;
//#define A13 = 15;
//#define A14 = 13;
//#define A15 = 12;
//#define A16 = 14;
//#define A17 = 27;
//#define A18 = 25;
//#define A19 = 26;

//******************************************************************************************
//Define which Arduino Pins will be used for each device
//******************************************************************************************
//"RESERVED" pins for ESP32 - best to avoid
#define PIN_0_RESERVED             0  //reserved ESP32 boot/program upload
#define PIN_1_RESERVED             1  //reserved ESP32 for TX0
#define PIN_3_RESERVED             3  //reserved ESP32 for RX0
#define PIN_6_RESERVED             6  //reserved ESP32 for flash
#define PIN_7_RESERVED             7  //reserved ESP32 for flash
#define PIN_8_RESERVED             8  //reserved ESP32 for flash
#define PIN_9_RESERVED             9  //reserved ESP32 for flash
#define PIN_10_RESERVED           10  //reserved ESP32 for flash
#define PIN_11_RESERVED           11  //reserved ESP32 for flash

//Analog Pins

//Digital Pins
#define PIN_RGB1          25  //(GPIO 25) SmartThings Capability "Color Control"
#define PIN_RGB2          26  //(GPIO 26) SmartThings Capability "Color Control"
#define PIN_RGBW1         27  //(GPIO 27) SmartThings Capability "Color Control"
#define PIN_RGBW2         28  //(GPIO 28) SmartThings Capability "Color Control"

//******************************************************************************************
//ESP32 WiFi Information
//******************************************************************************************
String str_ssid     = "YourSSID";                           //  <---You must edit this line!
String str_password = "YourPassword";                   //  <---You must edit this line!
IPAddress ip(192, 168, 1, 147);       //Device IP Address       //  <---You must edit this line!
IPAddress gateway(192, 168, 1, 1);    //Router gateway          //  <---You must edit this line!
IPAddress subnet(255, 255, 255, 0);   //LAN subnet mask         //  <---You must edit this line!
IPAddress dnsserver(192, 168, 1, 1);  //DNS server              //  <---You must edit this line!
const unsigned int serverPort = 8090; // port to run the http server on

// Smartthings / Hubitat Hub TCP/IP Address
IPAddress hubIp(192, 168, 1, 130);    // smartthings/hubitat hub ip //  <---You must edit this line!

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
// Serial.print(F("ST_Anything Callback: Sniffed data = "));
//  Serial.println(msg);
  //SerialBT.print(F("ST_Anything Callback: Sniffed data = "));
  //SerialBT.println(msg);  // For BlueTooth logging
    
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
  
  //Special sensors/executors (uses portions of both polling and executor classes)
  
  //Executors
  static st::EX_RGB_NeoPixelBus_T<NeoGrbFeature, NeoEsp32Rmt1800KbpsMethod>         executor1(F("rgbSwitch1"), 60, PIN_RGB1);  
  static st::EX_RGB_NeoPixelBus_T<NeoGrbFeature, NeoEsp32Rmt2800KbpsMethod>         executor2(F("rgbSwitch2"), 30, PIN_RGB2);  
  static st::EX_RGBW_NeoPixelBus_T<NeoGrbwFeature, NeoEsp32Rmt2800KbpsMethod>       executor3(F("rgbSwitch3"), 150, PIN_RGBW1);  
  static st::EX_RGBW_NeoPixelBus_T<NeoGrbwFeature, NeoEsp32Rmt2800KbpsMethod>       executor4(F("rgbSwitch3"), 60, PIN_RGBW2);  

  For the list of Features and Methods please see https://github.com/Makuna/NeoPixelBus/wiki/NeoPixelBus-object
  
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
  
  //Create the SmartThings ESP32WiFi Communications Object
    //STATIC IP Assignment - Recommended
    st::Everything::SmartThing = new st::SmartThingsESP32WiFi(str_ssid, str_password, ip, gateway, subnet, dnsserver, serverPort, hubIp, hubPort, st::receiveSmartString);
 
    //DHCP IP Assigment - Must set your router's DHCP server to provice a static IP address for this device's MAC address
    //st::Everything::SmartThing = new st::SmartThingsESP32WiFi(str_ssid, str_password, serverPort, hubIp, hubPort, st::receiveSmartString);

  //Run the Everything class' init() routine which establishes WiFi communications with SmartThings Hub
  st::Everything::init();
  
  //*****************************************************************************
  //Add each sensor to the "Everything" Class
  //*****************************************************************************
      
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

  //*****************************************************************************
  //Enable Bluetooth serial monitoring.  Uncomment to enable.
  //*****************************************************************************
  //Serial.begin(115200);
  //SerialBT.register_callback(callback);
  //if(!SerialBT.begin("ESP32_ST_Anything")){  //Name of your Bluetooth Signal.  Use a unique name if you have multiples
  //  Serial.println("An error occurred initializing Bluetooth");
  //}else{
  //  Serial.println("Bluetooth Device is Ready to Pair");
  //}
  
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

// Callback routine for BlueTooth.  Uncomment to enable 
//void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
//  if(event == ESP_SPP_SRV_OPEN_EVT){
//    Serial.println("Client Connected to BlueTooth!");
//    Serial.print("Client address is: ");
//     for (int i = 0; i < 6; i++) {
//      Serial.printf("%02X", param->srv_open.rem_bda[i]);
//      if (i < 6) {
//        Serial.print(":");
//      }
//     }
//  }
//
//  if(event == ESP_SPP_CLOSE_EVT ){
//    Serial.println("Client disconnected from BlueTooth");
//  }
//  
//}
