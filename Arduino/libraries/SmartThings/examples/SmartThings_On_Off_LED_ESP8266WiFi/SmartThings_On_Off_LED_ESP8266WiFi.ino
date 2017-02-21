//*****************************************************************************
/// @file
/// @brief
///   Arduino SmartThings Ethernet ESP8266 WiFi On/Off with LED Example 
///
///   Revised by Dan Ogorchock on 2017-02-11 to work with new "SmartThings v2.0" Library
///
///   Notes: The NodeMCU ESP communicates via WiFi to your home network router,
///          then to the ST Hub, and eventually to the ST cloud servers.
///
///
//*****************************************************************************

#include <SmartThingsESP8266WiFi.h>

//*****************************************************************************
// Pin Definitions    | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//                    V V V V V V V V V V V V V V V V V V V V V V V V V V V V V
//*****************************************************************************
//******************************************************************************************
//NodeMCU ESP8266 Pin Definitions (makes it much easier as these match the board markings)
//******************************************************************************************
#define LED_BUILTIN 16
#define BUILTIN_LED 16

#define D0 16
#define D1  5
#define D2  4
#define D3  0
#define D4  2
#define D5 14
#define D6 12
#define D7 13
#define D8 15
#define D9  3
#define D10 1


#define PIN_LED LED_BUILTIN  //Onboard LED

//*****************************************************************************
// Global Variables   | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//                    V V V V V V V V V V V V V V V V V V V V V V V V V V V V V
//*****************************************************************************
SmartThingsCallout_t messageCallout;    // call out function forward decalaration

//******************************************************************************************
//ESP8266 WiFi Information    CHANGE THIS INFORMATION ACCORDINGLY FOR YOUR NETWORK!
//******************************************************************************************
String str_ssid     = "yourSSIDhere";                           //  <---You must edit this line!
String str_password = "yourWiFiPasswordhere";                   //  <---You must edit this line!
IPAddress ip(192, 168, 1, 202);       // Device IP Address      //  <---You must edit this line!
IPAddress gateway(192, 168, 1, 1);    //router gateway          //  <---You must edit this line!
IPAddress subnet(255, 255, 255, 0);   //LAN subnet mask         //  <---You must edit this line!
IPAddress dnsserver(192, 168, 1, 1);  //DNS server              //  <---You must edit this line!
const unsigned int serverPort = 8090; // port to run the http server on

// Smartthings Hub Information
IPAddress hubIp(192, 168, 1, 149);    // smartthings hub ip     //  <---You must edit this line!
const unsigned int hubPort = 39500;   // smartthings hub port


//Create a SmartThings Ethernet ESP8266WiFi object
st::SmartThingsESP8266WiFi smartthing(str_ssid, str_password, ip, gateway, subnet, dnsserver, serverPort, hubIp, hubPort, messageCallout);

bool isDebugEnabled;    // enable or disable debug in this example


//*****************************************************************************
// Local Functions  | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//                  V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V
//*****************************************************************************
void on()
{
  digitalWrite(PIN_LED, LOW);  // turn LED on
  smartthing.send("on");        // send message to cloud
}

//*****************************************************************************
void off()
{
  digitalWrite(PIN_LED, HIGH);   // turn LED off
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

  if (isDebugEnabled)
  { // setup debug serial port
    Serial.begin(9600);         // setup serial with a baud rate of 9600
    Serial.println("");
    Serial.println("setup..");  // print out 'setup..' on start
  }
  
  // setup hardware pins 
  pinMode(PIN_LED, OUTPUT);     // define PIN_LED as an output
  digitalWrite(PIN_LED, HIGH);   // set value to HIGH (off)

  //Run the SmartThings init() routine to make sure the ThingShield is connected to the ST Hub
  smartthing.init();

  //synch up the ST cloud
  smartthing.send("off");       // send message to cloud
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

