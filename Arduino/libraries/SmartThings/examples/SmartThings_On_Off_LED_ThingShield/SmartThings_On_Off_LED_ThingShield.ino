//*****************************************************************************
/// @file
/// @brief
///   Arduino SmartThings ThingShield On/Off with LED Example 
///
///   Revised by Dan Ogorchock on 2017-02-10 to work with new "SmartThings v2.0" Library
///     -Now supports automatic selectic of SoftwareSerial or HardwareSerial constructor 
///      depending on the Arduino Model.
///     -If using an Arduino UNO R3, be sure to set the ThingShield switch to the D2/D3 position
///     -If using an Arduino Leonardo, be sure to set the ThingShield switch to the D0/D1 position
///     -If using an Arduino MEGA 2560, bbe sure to set the ThingShield switch to the D2/D3 position AND
///      connect jumper wires between pins 2 & 14 as well as between pins 3 & 15.
///
///      DO NOT USE PINS 0,1,2,3,6 for I/O in your sketch as pins 0,1,2,3 are used by the ThingShield
///      and USB communications to your computer. Pin 6 is reserved by the ThingShield.
///
/// @note  UNO R3 Pins
///              ______________
///             |              |
///             |         SW[] |
///             |[]RST         |
///             |         AREF |--
///             |          GND |--
///             |           13 |--X Onboard LED
///             |           12 |--
///             |           11 |--
///           --| 3.3V      10 |--
///           --| 5V         9 |--
///           --| GND        8 |--
///           --| GND          |
///           --| Vin        7 |--
///             |            6 |--Reserved
///           --| A0         5 |--
///           --| A1    ( )  4 |--
///           --| A2         3 |--X THING_RX
///           --| A3  ____   2 |--X THING_TX
///           --| A4 |    |  1 |--X THING_RX
///           --| A5 |    |  0 |--X THING_RX
///             |____|    |____|
///                  |____|
///
//*****************************************************************************
#include <SmartThingsThingShield.h>

//*****************************************************************************
// Pin Definitions    | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//                    V V V V V V V V V V V V V V V V V V V V V V V V V V V V V
//*****************************************************************************
#define PIN_LED         13
#define PIN_THING_RX    3
#define PIN_THING_TX    2

//*****************************************************************************
// Global Variables   | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//                    V V V V V V V V V V V V V V V V V V V V V V V V V V V V V
//*****************************************************************************
SmartThingsCallout_t messageCallout;    // call out function forward decalaration

  #if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_MINI) //Arduino UNO, NANO, MINI
    st::SmartThingsThingShield smartthing(PIN_THING_RX, PIN_THING_TX, messageCallout);     //Use Software Serial constructor
  #elif defined(ARDUINO_AVR_LEONARDO)                                                    //Arduino Leonardo
     st::SmartThingsThingShield smartthing(&Serial1, messageCallout);                      //Use Hardware Serial constructor w/Serial1
  #elif defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560)                       //Arduino MEGA 1280 or 2560
     st::SmartThingsThingShield smartthing(&Serial3, messageCallout);                      //Use Hardware Serial constructor w/Serial3
  #else  
    //assume user is using an UNO for the unknown case
    st::SmartThingsThingShield smartthing(PIN_THING_RX, PIN_THING_TX, messageCallout);  //Use Software Serial constructor
  #endif  

bool isDebugEnabled;    // enable or disable debug in this example
int stateLED;           // state to track last set value of LED

//*****************************************************************************
// Local Functions  | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//                  V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V
//*****************************************************************************
void on()
{
  stateLED = 1;                 // save state as 1 (on)
  digitalWrite(PIN_LED, HIGH);  // turn LED on
  smartthing.shieldSetLED(0, 0, 1);
  smartthing.send("on");        // send message to cloud
}

//*****************************************************************************
void off()
{
  stateLED = 0;                 // set state to 0 (off)
  digitalWrite(PIN_LED, LOW);   // turn LED off
  smartthing.shieldSetLED(0, 0, 0);
  smartthing.send("off");       // send message to cloud
}

//*****************************************************************************
// API Functions    | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//                  V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V
//*****************************************************************************
void setup()
{
  // setup default state of global variables
  isDebugEnabled = true;
  stateLED = 0;                 // matches state of hardware pin set below
  
  // setup hardware pins 
  pinMode(PIN_LED, OUTPUT);     // define PIN_LED as an output
  digitalWrite(PIN_LED, LOW);   // set value to LOW (off) to match stateLED=0

  //Run the SmartThings init() routine to make sure the ThingShield is connected to the ST Hub
  smartthing.init();
  
  if (isDebugEnabled)
  { // setup debug serial port
    Serial.begin(9600);         // setup serial with a baud rate of 9600
    Serial.println("setup..");  // print out 'setup..' on start
  }
}

//*****************************************************************************
void loop()
{
  // run smartthing logic
  smartthing.run();
}

//*****************************************************************************
void messageCallout(String message)
{
  // if debug is enabled print out the received message
  if (isDebugEnabled)
  {
    Serial.print("Received message: '");
    Serial.print(message);
    Serial.println("' ");
  }

  // if message contents equals to 'on' then call on() function
  // else if message contents equals to 'off' then call off() function
  if (message.equals("on"))
  {
    on();
  }
  else if (message.equals("off"))
  {
    off();
  }
}

