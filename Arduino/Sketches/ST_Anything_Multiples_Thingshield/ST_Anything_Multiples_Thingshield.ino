//******************************************************************************************
//  File: ST_Anything_Multiples_Thingshield.ino
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one Arduino + SmartThings Shield to 
//            implement a multi input/output custom device for integration into SmartThings.
//            The ST_Anything library takes care of all of the work to schedule device updates
//            as well as all communications with the SmartThings Shield.
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
//              - 2 x Dimmer Switch devices - uses 2 digital outputs, one for on/off and one for pwm level
//              - 2 x MQ-2 Smoke Detector devices (using simple analog input compared to user defined limit)
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
//            Note: We did not have a Leonardo for testing, but did fully test on UNO R3 and 
//            MEGA 2560 using both SoftwareSerial and Hardware Serial communications to the 
//            Thing Shield.  This sketch, due to the number of devices implemented, requires an 
//            Arduino MEGA 2560 wuth ThingShield wired to "Serial3"  
//    
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//    2015-03-28  Dan Ogorchock  Removed RCSwitch #include now that the libraries are split up
//    2015-03-31  Daniel O.      Memory optimizations utilizing progmem
//    2015-08-23  Dan Ogorchock  Added optional alarm limit argument to water sensor
//    2017-02-12  Dan Ogorchock  Revised to use the new SMartThings v2.0 library
//    2017-04-22  Dan Ogorchock  Added Voltage, Carbon Monoxide, and Alarm with Strobe
//    2017-04-25  Dan Ogorchock  Revised for Parent/Child Composite Device Handler
//    2017-11-21  Dan Ogorchock  Fixed Arduino UNO compatability (missing pinRx and pinTx definitions)
//
//******************************************************************************************
//******************************************************************************************
// SmartThings Library for Arduino Shield
//******************************************************************************************
#include <SmartThingsThingShield.h>    //Library to provide API to the SmartThings ThingShield

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

//******************************************************************************************
//Define which Arduino Pins will be used for each device
//  Notes: -Serial Communications Pins are defined in Constants.h (avoid pins 0,1,2,3
//          for inputs and output devices below as they may be used for communications)
//         -Always avoid Pin 6 as it is reserved by the SmartThings Shield
//
//******************************************************************************************
//"RESERVED" pins for SmartThings ThingShield - best to avoid
#define PIN_O_RESERVED           0  //reserved by ThingShield for Serial communications OR USB Serial Monitor
#define PIN_1_RESERVED           1  //reserved by ThingShield for Serial communications OR USB Serial Monitor
#define pinTX                    2  //reserved by ThingShield for Serial communications
#define pinRX                    3  //reserved by ThingShield for Serial communications
#define PIN_6_RESERVED           6  //reserved by ThingShield (possible future use?)

//Analog Pins
#define PIN_WATER_1               A0  //SmartThings Capability "Water Sensor"
#define PIN_WATER_2               A1  //SmartThings Capability "Water Sensor"
#define PIN_ILLUMINANCE_1         A2  //SmartThings Capability "Illuminance Measurement"
#define PIN_ILLUMINANCE_2         A3  //SmartThings Capability "Illuminance Measurement"
#define PIN_VOLTAGE_1             A4  //SmartThings Capability "Voltage Measurement"
#define PIN_VOLTAGE_2             A5  //SmartThings Capability "Voltage Measurement"
#if not defined(ARDUINO_AVR_UNO)
#define PIN_SMOKE_3               A8  //SmartThings Capability "Smoke Detector"
#define PIN_SMOKE_4               A9  //SmartThings Capability "Smoke Detector"
#endif

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

//Dimmer Switch Pins
#define PIN_DIMMERLEVEL_1         44  //SmartThings Capability "Switch Level"  NOTE: MUST BE A PWM CAPABLE PIN!
#define PIN_DIMMERSWITCH_1        45  //SmartThings Capability "Switch"
#define PIN_DIMMERLEVEL_2         46  //SmartThings Capability "Switch Level"  NOTE: MUST BE A PWM CAPABLE PIN!
#define PIN_DIMMERSWITCH_2        47  //SmartThings Capability "Switch"

//Garage Door Pins 
#define PIN_DOORCONTROL_CONTACT_1 35  //SmartThings Capabilty "Door Control" 
#define PIN_DOORCONTROL_RELAY_1   36  //SmartThings Capabilty "Door Control" 
#define PIN_DOORCONTROL_CONTACT_2 37  //SmartThings Capabilty "Door Control"  
#define PIN_DOORCONTROL_RELAY_2   38  //SmartThings Capabilty "Door Control" 

//Pushbutton Pins
#define PIN_BUTTON1               48  //SmartThings Capabilty Button / Holdable Button
#define PIN_BUTTON2               49  //SmartThings Capabilty Button / Holdable Button

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
#if not defined(ARDUINO_AVR_UNO)
  static st::PS_MQ2_Smoke           sensor23(F("smoke3"), 10, 3, PIN_SMOKE_3, 300);
  static st::PS_MQ2_Smoke           sensor24(F("smoke4"), 10, 5, PIN_SMOKE_4, 300);
#endif
  
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
  static st::EX_Switch_Dim          executor5(F("dimmerSwitch1"), PIN_DIMMERSWITCH_1, PIN_DIMMERLEVEL_1, LOW, false);   
  static st::EX_Switch_Dim          executor6(F("dimmerSwitch2"), PIN_DIMMERSWITCH_2, PIN_DIMMERLEVEL_2, LOW, false);    
    
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

  //Create the SmartThings Thingshield Communications Object based on Arduino Model
  #if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_MINI)   //Arduino UNO, NANO, MINI
    st::Everything::SmartThing = new st::SmartThingsThingShield(pinRX, pinTX, st::receiveSmartString);      //Use Software Serial
  #elif defined(ARDUINO_AVR_LEONARDO)  //Arduino Leonardo
    st::Everything::SmartThing = new st::SmartThingsThingShield(&Serial1, st::receiveSmartString);  //Use Hardware Serial
  #elif defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560)  //Arduino MEGA 1280 or 2560
    st::Everything::SmartThing = new st::SmartThingsThingShield(&Serial3, st::receiveSmartString);  //Use Hardware Serial
  #else  
    //assume user is using an UNO for the unknown case
    st::Everything::SmartThing = new st::SmartThingsThingShield(pinRX, pinTX, st::receiveSmartString);               //Software Serial
  #endif  

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
  st::Everything::addSensor(&sensor15); 
  st::Everything::addSensor(&sensor16); 
  st::Everything::addSensor(&sensor17); 
  st::Everything::addSensor(&sensor18); 
  st::Everything::addSensor(&sensor19); 
  st::Everything::addSensor(&sensor20); 
  st::Everything::addSensor(&sensor21); 
  st::Everything::addSensor(&sensor22); 
#if not defined(ARDUINO_AVR_UNO)
  st::Everything::addSensor(&sensor23);
  st::Everything::addSensor(&sensor24);
#endif
    
  //*****************************************************************************
  //Add each executor to the "Everything" Class
  //*****************************************************************************
  st::Everything::addExecutor(&executor1);
  st::Everything::addExecutor(&executor2);
  st::Everything::addExecutor(&executor3);
  st::Everything::addExecutor(&executor4);
  st::Everything::addExecutor(&executor5);
  st::Everything::addExecutor(&executor6);
  
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
