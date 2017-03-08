//******************************************************************************************
//  File: ST_Anything_Doors_Windows_EthernetW5100.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one Arduino + W5100 Ethernet Shield to 
//            implement a multi input/output custom device for integration into SmartThings.
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the W5100 Shield.
//
//            ST_Anything_Doors_Example implements the following:
//              - 13 x Contact Sensor devices (used to monitor magnetic door/windows sensors)
//              - 1 x Motion device (used to detect motion)
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
//    
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//    2015-01-07  Dan Ogorchock  Modified for Door Monitoring and Garage Door Control
//    2015-03-28  Dan Ogorchock  Removed RCSwitch #include now that the libraries are split up
//    2015-03-31  Daniel O.      Memory optimizations utilizing progmem
//    2015-10-31  Dan Ogorchock  Revised for a specific user request
//    2017-02-12  Dan Ogorchock  Revised to use the new SMartThings v2.0 library
//    2017-02-13  Dan Ogorchock  Ported to use the W5100 Ethernet Shield
//
//******************************************************************************************

//******************************************************************************************
// SmartThings Library for Arduino Shield
//******************************************************************************************
#include <SmartThingsEthernetW5100.h>    //Library to provide API to the SmartThings ThingShield

//******************************************************************************************
// ST_Anything Library 
//******************************************************************************************
#include <Constants.h>       //Constants.h is designed to be modified by the end user to adjust behavior of the ST_Anything library
#include <Device.h>          //Generic Device Class, inherited by Sensor and Executor classes
#include <Sensor.h>          //Generic Sensor Class, typically provides data to ST Cloud (e.g. Temperature, Motion, etc...)
#include <InterruptSensor.h> //Generic Interrupt "Sensor" Class, waits for change of state on digital input 
#include <Everything.h>      //Master Brain of ST_Anything library that ties everything together and performs ST Shield communications

#include <IS_Motion.h>       //Implements an Interrupt Sensor (IS) to detect motion via a PIR sensor
#include <IS_Contact.h>      //Implements an Interrupt Sensor (IS) to monitor the status of a digital input pin

//**********************************************************************************************************
//Define which Arduino Pins will be used for each device
//  Notes: Arduino communicates with both the W5100 and SD card using the SPI bus (through the ICSP header). 
//         This is on digital pins 10, 11, 12, and 13 on the Uno and pins 50, 51, and 52 on the Mega. 
//         On both boards, pin 10 is used to select the W5100 and pin 4 for the SD card. 
//         These pins cannot be used for general I/O. On the Mega, the hardware SS pin, 53, 
//         is not used to select either the W5100 or the SD card, but it must be kept as an output 
//         or the SPI interface won't work.
//         See https://www.arduino.cc/en/Main/ArduinoEthernetShieldV1 for details on the W5100 Sield
//**********************************************************************************************************
//"RESERVED" pins for W5100 Ethernet Shield - best to avoid
#define PIN_4_RESERVED           4   //reserved by W5100 Shield on both UNO and MEGA
#define PIN_1O_RESERVED          10  //reserved by W5100 Shield on both UNO and MEGA
#define PIN_11_RESERVED          11  //reserved by W5100 Shield on UNO
#define PIN_12_RESERVED          12  //reserved by W5100 Shield on UNO
#define PIN_13_RESERVED          13  //reserved by W5100 Shield on UNO
#define PIN_50_RESERVED          50  //reserved by W5100 Shield on MEGA
#define PIN_51_RESERVED          51  //reserved by W5100 Shield on MEGA
#define PIN_52_RESERVED          52  //reserved by W5100 Shield on MEGA
#define PIN_53_RESERVED          53  //reserved by W5100 Shield on MEGA

//Pin selections below are for an Arduino MEGA 2560.  Adjust accordingly for an UNO.

//Window Pins
#define PIN_CONTACT_KITCHEN_WINDOW1  22
#define PIN_CONTACT_KITCHEN_WINDOW2  23
#define PIN_CONTACT_KITCHEN_WINDOW3  24
#define PIN_CONTACT_MASTER_WINDOW1   25
#define PIN_CONTACT_MASTER_WINDOW2   26
#define PIN_CONTACT_OFFICE_WINDOW1   27
#define PIN_CONTACT_OFFICE_WINDOW2   28
#define PIN_CONTACT_GUEST_WINDOW1    29
#define PIN_CONTACT_GUEST_WINDOW2    30


//House Door Pins
#define PIN_CONTACT_FRONT_DOOR       31
#define PIN_CONTACT_KITCHEN_DOOR     32
#define PIN_CONTACT_GARAGE_DOOR      33
#define PIN_CONTACT_BEDROOM_DOOR     34

//motion pins
#define PIN_MOTION                   35

//******************************************************************************************
//W5100 Ethernet Shield Information
//****************************************************************************************** 
byte mac[] = {0x06,0x02,0x03,0x04,0x05,0x06}; //MAC address, leave first octet 0x06, change others to be unique //  <---You must edit this line!
IPAddress ip(192, 168, 1, 200);               //Arduino device IP Address                   //  <---You must edit this line!
IPAddress gateway(192, 168, 1, 1);            //router gateway                              //  <---You must edit this line!
IPAddress subnet(255, 255, 255, 0);           //LAN subnet mask                             //  <---You must edit this line!
IPAddress dnsserver(192, 168, 1, 1);          //DNS server                                  //  <---You must edit this line!
const unsigned int serverPort = 8090;         // port to run the http server on

// Smartthings hub information
IPAddress hubIp(192,168,1,149);               // smartthings hub ip                         //  <---You must edit this line!
const unsigned int hubPort = 39500;           // smartthings hub port

//******************************************************************************************
//Arduino Setup() routine
//******************************************************************************************
void setup()
{
  //****************************************************************************************
  //Declare each Device that is attached to the Arduino
  //  Notes: - For each device, there is typically a corresponding "tile" defined in your 
  //           SmartThings DeviceType Groovy code
  //         - For details on each device's constructor arguments below, please refer to the 
  //           corresponding header (.h) and program (.cpp) files.
  //         - The name assigned to each device (1st argument below) must match the Groovy
  //           DeviceType Tile name.  
  //****************************************************************************************
 
  static st::IS_Motion sensor1(F("motion"), PIN_MOTION, HIGH, false);
  static st::IS_Contact sensor2(F("kitchenWindow1"), PIN_CONTACT_KITCHEN_WINDOW1, LOW, true, 500);
  static st::IS_Contact sensor3(F("kitchenWindow2"), PIN_CONTACT_KITCHEN_WINDOW2, LOW, true, 500);
  static st::IS_Contact sensor4(F("kitchenWindow3"), PIN_CONTACT_KITCHEN_WINDOW3, LOW, true, 500);
  static st::IS_Contact sensor5(F("masterWindow1"), PIN_CONTACT_MASTER_WINDOW1, LOW, true, 500);
  static st::IS_Contact sensor6(F("masterWindow2"), PIN_CONTACT_MASTER_WINDOW2, LOW, true, 500);
  static st::IS_Contact sensor7(F("officeWindow1"), PIN_CONTACT_OFFICE_WINDOW1, LOW, true, 500);
  static st::IS_Contact sensor8(F("officeWindow2"), PIN_CONTACT_OFFICE_WINDOW2, LOW, true, 500);
  static st::IS_Contact sensor9(F("guestWindow1"), PIN_CONTACT_GUEST_WINDOW1, LOW, true, 500);
  static st::IS_Contact sensor10(F("guestWindow2"), PIN_CONTACT_GUEST_WINDOW2, LOW, true, 500);
  static st::IS_Contact sensor11(F("frontDoor"), PIN_CONTACT_FRONT_DOOR, LOW, true, 500);
  static st::IS_Contact sensor12(F("kitchenDoor"), PIN_CONTACT_KITCHEN_DOOR, LOW, true, 500);
  static st::IS_Contact sensor13(F("garageDoor"), PIN_CONTACT_GARAGE_DOOR, LOW, true, 500);
  static st::IS_Contact sensor14(F("bedroomDoor"), PIN_CONTACT_BEDROOM_DOOR, LOW, true, 500);
  
  //*****************************************************************************
  //  Configure debug print output from each main class 
  //  -Note: Set these to "false" if using Hardware Serial on pins 0 & 1
  //         to prevent communication conflicts with the ST Shield communications
  //*****************************************************************************
  st::Everything::debug=true;
  st::Device::debug=true;
  st::InterruptSensor::debug=true;
  
  //*****************************************************************************
  //Initialize the "Everything" Class
  //*****************************************************************************

  //Create the SmartThings EthernetW5100 Communications Object
  st::Everything::SmartThing = new st::SmartThingsEthernetW5100(mac, ip, gateway, subnet, dnsserver, serverPort, hubIp, hubPort, st::receiveSmartString);

  //Run the Everything class' init() routine which establishes communications with SmartThings
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
  
  //*****************************************************************************
  //Initialize each of the devices which were added to the Everything Class
  //*****************************************************************************
  st::Everything::initDevices();
}

//*******************************************************************************
//Arduino Loop() routine
//*******************************************************************************
void loop()
{
  //*****************************************************************************
  //Execute the Everything run method which takes care of "Everything"
  //*****************************************************************************
  st::Everything::run();
}
