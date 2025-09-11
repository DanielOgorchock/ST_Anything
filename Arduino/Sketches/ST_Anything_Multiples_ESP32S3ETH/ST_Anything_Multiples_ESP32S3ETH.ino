//******************************************************************************************
//  File: ST_Anything_Multiples_ESP32S3ETH.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one ESP32 to implement 
//            a multi input/output custom device for integration into Hubitat.
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the ESP32-S3-ETH's W5500 Ethernet.
//
//            ST_Anything_Multiples implements the following ST Capabilities as a demo of what is possible with a single ESP32
//              - 1 x Door Control devices (used typically for Garage Doors - input pin (contact sensor) and output pin (relay switch)
//              - 2 x Contact Sensor devices (used to monitor magnetic door sensors)
//              - 1 x Switch devices (used to turn on a digital output (e.g. LED, relay, etc...)
//              - 1 x Smoke Detector device (using simple digital input)
//              - 2 x Motion devices (used to detect motion)
//              - 1 x Temperature Measurement device (Temperature from DHT22 device)
//              - 1 x Humidity Measurement device (Humidity from DHT22 device)
//              - 1 x Temperature Measurement device (Temperature from Dallas Semi 1-Wire DS18B20 device)
//              - 1 x Relay Switch device (used to turn on a digital output for a set number of cycles And On/Off times (e.g.relay, etc...))
//              - 2 x Button devices (sends "pushed" if held for less than 1 second, else sends "held"
//              - 2 x Alarm devices - 1 siren only, 1 siren and strobe (using simple digital outputs)
//
//    
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2017-08-14  Dan Ogorchock  Original Creation - Adapted from ESP8266 to work with ESP32 board
//    2018-02-09  Dan Ogorchock  Added support for Hubitat Elevation Hub
//    2025-09-11  Dan Ogorchock  Ported to run on the Waveshare ESP32-S3-ETH microcontroller
//
//   Special thanks to Joshua Spain for his contributions in porting ST_Anything to the ESP32!
//
//******************************************************************************************
//******************************************************************************************
// SmartThings Library for ESP32-S3-ETH W5500 Ethernet 
//******************************************************************************************
#include <SmartThingsESP32S3ETH.h>

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

#include <PS_DS18B20_Temperature.h>  //Implements a Polling Sesnor (PS) to measure Temperature via DS18B20 libraries 
#include <PS_Illuminance.h>  //Implements a Polling Sensor (PS) to measure light levels via a photo resistor on an analog input pin 
#include <PS_Voltage.h>      //Implements a Polling Sensor (PS) to measure voltage on an analog input pin 
#include <PS_TemperatureHumidity.h>  //Implements a Polling Sensor (PS) to measure Temperature and Humidity via DHT library
#include <PS_Water.h>        //Implements a Polling Sensor (PS) to measure presence of water (i.e. leak detector) on an analog input pin 
#include <PS_MQ2_Smoke.h>    //Implements an Polling Sensor (PS) to monitor the status of an analog input pin from a MQ2 sensor
#include <IS_Motion.h>       //Implements an Interrupt Sensor (IS) to detect motion via a PIR sensor on a digital input pin
#include <IS_Contact.h>      //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin
#include <IS_Smoke.h>        //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin
#include <IS_CarbonMonoxide.h> //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin
#include <IS_DoorControl.h>  //Implements an Interrupt Sensor (IS) and Executor to monitor the status of a digital input pin and control a digital output pin
#include <IS_Button.h>       //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin for button presses
#include <EX_Switch.h>       //Implements an Executor (EX) via a digital output to a relay
#include <EX_Alarm.h>        //Implements Executor (EX)as an Alarm capability with Siren and Strobe via digital outputs to relays
#include <S_TimedRelay.h>    //Implements a Sensor to control a digital output pin with timing/cycle repeat capabilities
#include <EX_Switch_Dim.h>   //Implements an Executor (EX) for a switch (on/off) and pwm output (level) uses 2 digital output pins
#include <IS_Presence.h>

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
//ESP32-S3-ETH  
//****************************************************************************************************************************

// Not sure if this board has ADC pins or not?  Board resets itself when I try to use any ADC GPIO Pins.  DGO 2025-09-11  
// #define A1    1
// #define A2    2
// #define A3    3
// #define A15  15
// #define A16  16
// #define A17  17
// #define A18  18

#define D0    0
#define D1    1
#define D2    2
#define D3    3
#define D15  15
#define D16  16
#define D17  17
#define D18  18
#define D21  21
#define D33  33
#define D34  34
#define D35  35
#define D36  36
#define D37  37
#define D38  38
#define D39  39
#define D40  40
#define D41  41
#define D42  42
#define D45  45
#define D46  46
#define D47  47
#define D48  48


//******************************************************************************************
//Define which Arduino Pins will be used for each device
//******************************************************************************************
//"RESERVED" pins for ESP32-S3-ETH board - best to avoid
#define PIN_43_RESERVED            43 //reserved ESP32 for UART0 TX
#define PIN_44_RESERVED            44 //reserved ESP32 for UART0 RX
#define PIN_4_RESERVED             4  //reserved ESP32 for SD_CS
#define PIN_5_RESERVED             5  //reserved ESP32 for SD_MISO
#define PIN_6_RESERVED             6  //reserved ESP32 for SD_MOSI
#define PIN_7_RESERVED             7  //reserved ESP32 for SD_CLK
#define PIN_9_RESERVED             9  //reserved ESP32 for ETH_RST
#define PIN_10_RESERVED           10  //reserved ESP32 for ETH_INT
#define PIN_11_RESERVED           11  //reserved ESP32 for ETH_MOSI
#define PIN_12_RESERVED           12  //reserved ESP32 for ETH_MISO
#define PIN_13_RESERVED           13  //reserved ESP32 for ETH_CLK
#define PIN_14_RESERVED           14  //reserved ESP32 for ETH_CS
#define PIN_19_RESERVED           19  //reserved ESP32 for USB D_N
#define PIN_20_RESERVED           20  //reserved ESP32 for USB D_P

//Analog Pins - These do not seem to work in my limited testing
// #define PIN_WATER_1               A1  //(GPIO 1) Hubitat Capability "Water Sensor"
// #define PIN_WATER_2               A2  //(GPIO 2) Hubitat Capability "Water Sensor"
// #define PIN_ILLUMINANCE_1         A3  //(GPIO 3) Hubitat Capability "Illuminance Measurement"
// #define PIN_ILLUMINANCE_2         A15  //(GPIO 15) Hubitat Capability "Illuminance Measurement"
// #define PIN_VOLTAGE_1             A16  //(GPIO 16) Hubitat Capability "Voltage Measurement"
// #define PIN_SMOKE_1               A17  //(GPIO 17) Hubitat Capability "Smoke Detector" (MQ-2)

//Digital Pins
#define PIN_TEMPERATUREHUMIDITY_1 D21  //Hubitat Capabilities "Temperature Measurement" and "Relative Humidity Measurement"
#define PIN_TEMPERATURE_2         D33  //Hubitat Capabilty "Temperature Measurement" (Dallas Semiconductor DS18B20)

#define PIN_MOTION_1              D34  //Hubitat Capability "Motion Sensor"
#define PIN_MOTION_2              D35  //Hubitat Capability "Motion Sensor"
#define PIN_CONTACT_1             D36  //Hubitat Capability "Contact Sensor"
#define PIN_CONTACT_2             D37  //Hubitat Capability "Contact Sensor"
#define PIN_SWITCH_1              D38  //Hubitat Capability "Switch"
#define PIN_SMOKE_2               D39  //Hubitat Capability "Smoke Detector"
#define PIN_ALARM_1               D40  //Hubitat Capability "Alarm"
#define PIN_ALARM_2               D41  //Hubitat Capability "Alarm"
#define PIN_STROBE_2              D42  //Hubitat Capability "Alarm"              
#define PIN_DOORCONTROL_CONTACT_1 D45  //Hubitat Capabilty "Door Control" 
#define PIN_DOORCONTROL_RELAY_1   D46  //Hubitat Capabilty "Door Control" 
#define PIN_BUTTON_1              D47  //Hubitat Capabilty Button / Holdable Button
#define PIN_BUTTON_2              D48  //Hubitat Capabilty Button / Holdable Button
#define PIN_TIMEDRELAY_1          D0   //Hubitat Capability "Relay Switch"


//******************************************************************************************
//ESP32-S3-ETH Ethernet Information
//******************************************************************************************

//NOTE - If your shield came with a MAC address sticker - please use that MAC address!
byte mac[] = {0x06,0x02,0x03,0x04,0x05,0x06}; //MAC address, leave first octet 0x06, change others to be unique //  <---You must edit this line!

IPAddress ip(192, 168, 1, 233);       //Device IP Address       //  <---You must edit this line!
IPAddress gateway(192, 168, 1, 1);    //Router gateway          //  <---You must edit this line!
IPAddress subnet(255, 255, 255, 0);   //LAN subnet mask         //  <---You must edit this line!
IPAddress dnsserver(192, 168, 1, 1);  //DNS server              //  <---You must edit this line!
const unsigned int serverPort = 8090; // port to run the http server on

// Hubitat Hub TCP/IP Address
IPAddress hubIp(192, 168, 1, 148);    // hubitat hub ip //  <---You must edit this line!

// Hubitat Hub TCP/IP Port: 
const unsigned int hubPort = 39501;   // hubitat hub port

//******************************************************************************************
//st::Everything::callOnMsgSend() optional callback routine.  This is a sniffer to monitor 
//    data being sent to ST.  This allows a user to act on data changes locally within the 
//    Arduino sktech.
//******************************************************************************************
void callback(const String &msg)
{
//  String strTemp = msg;
//  Serial.print(F("ST_Anything Callback: Sniffed data = "));  //Standard serial port monitoring
//  Serial.println(msg);
//  SerialBT.print(F("ST_Anything Callback: Sniffed data = "));  // BlueTooth monitoring
//  SerialBT.println(msg);
  
  //TODO:  Add local logic here to take action when a device's value/state is changed
  
  //Masquerade as the ThingShield to send data to the Arduino, as if from the ST Cloud (uncomment and edit following line)
  //st::receiveSmartString("Put your command here!");  //use same strings that the Device Handler would send
//  if (strTemp.startsWith("temperature1"))
//  {
//    strTemp.remove(0,13);
//    Serial.println(strTemp);
//  }
//  if (strTemp.startsWith("humidity1"))
//  {
//    strTemp.remove(0,10);
//    Serial.println(strTemp);
//  }
}

//******************************************************************************************
//Arduino Setup() routine
//******************************************************************************************
void setup()
{
  //******************************************************************************************
  // Setup the default values for the ADC.  Used for analog voltage reads.
  //  Notes:  analogReadResolution(12) sets the resolution for all pins. 12 = 0-4095, 11 = 0-2047, 10 = 0-1024, 9 = 0-512 
  //          analogSetAttenuation(ADC_11db) sets the attenuation for all pins.  11db = 0-3.3v, 6dB range = 0-2.2v, 2.5db = 0-1.5v, 0db = 0-1v
  //          analogSetPinAttenuation(A7, ADC_11db) sets the attenuation for individual pins.
  //******************************************************************************************
  
  // analogReadResolution(12);        // Default of 12 is not very linear.  Recommended to use 10 or 11 depending on needed resolution.
  // analogSetAttenuation(ADC_6db);   // Default is 11db which is very noisy.  Recommended to use 2.5 or 6.
  
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
   static st::PS_TemperatureHumidity sensor1(F("temphumid1"), 15, 5, PIN_TEMPERATUREHUMIDITY_1, st::PS_TemperatureHumidity::DHT22,"temperature1","humidity1");
   static st::PS_DS18B20_Temperature sensor2(F("temperature2"), 60, 55, PIN_TEMPERATURE_2, false, 10, 1); 

  //Interrupt Sensors 
   static st::IS_Motion              sensor3(F("motion1"), PIN_MOTION_1, HIGH, false, 500);
   static st::IS_Motion              sensor4(F("motion2"), PIN_MOTION_2, HIGH, false, 500);
   static st::IS_Contact             sensor5(F("contact1"), PIN_CONTACT_1, LOW, true, 500);
   static st::IS_Contact             sensor6(F("contact2"), PIN_CONTACT_2, LOW, true, 500);
   static st::IS_Smoke               sensor7(F("smoke2"), PIN_SMOKE_2, HIGH, true, 500);
   static st::IS_Button              sensor8(F("button1"), PIN_BUTTON_1, 1000, LOW, true, 500);
   static st::IS_Button              sensor9(F("button2"), PIN_BUTTON_2, 1000, LOW, true, 500);
  
  //Special sensors/executors (uses portions of both polling and executor classes)
   static st::IS_DoorControl         sensor10(F("doorControl1"), PIN_DOORCONTROL_CONTACT_1, LOW, true, PIN_DOORCONTROL_RELAY_1, LOW, true, 1000);
   static st::S_TimedRelay           sensor11(F("relaySwitch1"), PIN_TIMEDRELAY_1, LOW, false, 3000, 0, 1);
  
  //Executors
   static st::EX_Switch              executor1(F("switch1"), PIN_SWITCH_1, LOW, true);
   static st::EX_Alarm               executor2(F("alarm1"), PIN_ALARM_1, LOW, true);
   static st::EX_Alarm               executor3(F("alarm2"), PIN_ALARM_2, LOW, true, PIN_STROBE_2);
  
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
    st::Everything::SmartThing = new st::SmartThingsESP32S3ETH(mac, ip, gateway, subnet, dnsserver, serverPort, hubIp, hubPort, st::receiveSmartString);

    //DHCP IP Assigment - Must set your router's DHCP server to provice a static IP address for this device's MAC address
    //st::Everything::SmartThing = new st::SmartThingsESP32S3ETH(mac, serverPort, hubIp, hubPort, st::receiveSmartString);

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
  st::Everything::addSensor(&sensor9);
  st::Everything::addSensor(&sensor10);
  st::Everything::addSensor(&sensor11);
      
  //*****************************************************************************
  //Add each executor to the "Everything" Class
  //*****************************************************************************
 st::Everything::addExecutor(&executor1);
 st::Everything::addExecutor(&executor2);
 st::Everything::addExecutor(&executor3);
      
  //*****************************************************************************
  //Initialize each of the devices which were added to the Everything Class
  //*****************************************************************************
  st::Everything::initDevices();
  
  //*****************************************************************************
  //Enable Bluetooth serial monitoring.  Uncomment to use.
  //*****************************************************************************
  //Serial.begin(115200);
  //SerialBT.register_callback(callback);
  //if(!SerialBT.begin("ESP32_ST_Anything")){  //Name of your Bluetooth Signal.  Use a unique name if you have multiple devices
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
//  if(event == ESP_SPP_CLOSE_EVT ){
//    Serial.println("Client disconnected from BlueTooth");
//  }
// }
