//*****************************************************************************
/// @file
/// @brief
///   Arduino SmartThings Ethernet WiFiEsp On/Off with LED Example 
///
///   Revised by Dan Ogorchock on 2017-02-20 to work with new "SmartThings v2.0" Library
///
///   Notes: The Arduino MEGA 2560 communicates via Serial1 to the ESP-01 WiFi board 
///          to your home network router, then to the ST Hub, and eventually to the 
///          ST cloud servers.
///    
///          You must connect the ESP-01 to the Arduino MEGA 2560's "Serial1" 
///          Hardware Serial port on pins 18 & 19.  Google it!
///
///          The ESP-01 must be running a version of firmware compatible with
///          the WiFiEsp library.  Try the examples in the WiFiEsp library
///          to make sure everything is wried correctly before trying this sketch!
///
//*****************************************************************************

#include <SmartThingsWiFiEsp.h>

//*****************************************************************************
// Pin Definitions    | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//                    V V V V V V V V V V V V V V V V V V V V V V V V V V V V V
//*****************************************************************************
#define PIN_LED         13

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
IPAddress ip(192, 168, 1, 203);       // Device IP Address      //  <---You must edit this line!
const unsigned int serverPort = 8090; // port to run the http server on

// Smartthings Hub Information
IPAddress hubIp(192, 168, 1, 149);    // smartthings hub ip     //  <---You must edit this line!
const unsigned int hubPort = 39500;   // smartthings hub port


//Create a SmartThings Ethernet ESP8266WiFi object using the Arduino + ESP-01 constructor
st::SmartThingsWiFiEsp smartthing(&Serial1, str_ssid, str_password, ip, serverPort, hubIp, hubPort, messageCallout);

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
  //smartthing.send("on");        // send message to cloud
}

//*****************************************************************************
void off()
{
  stateLED = 0;                 // set state to 0 (off)
  digitalWrite(PIN_LED, LOW);   // turn LED off
  //smartthing.send("off");       // send message to cloud
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

  if (isDebugEnabled)
  { // setup debug serial port
    Serial.begin(9600);         // setup serial with a baud rate of 9600
    Serial.println("");
    Serial.println("setup..");  // print out 'setup..' on start
  }
  
  // setup hardware pins 
  pinMode(PIN_LED, OUTPUT);     // define PIN_LED as an output
  digitalWrite(PIN_LED, LOW);   // set value to LOW (off) to match stateLED=0

  // initialize Hardware Serial UART for ESP module
  Serial1.begin(115200); //May need to adjust the baud rate for your ESP-01 (9600, 57600, 115200)
  
  //Run the SmartThings init() routine to make sure the ThingShield is connected to the ST Hub
  smartthing.init();
  

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

