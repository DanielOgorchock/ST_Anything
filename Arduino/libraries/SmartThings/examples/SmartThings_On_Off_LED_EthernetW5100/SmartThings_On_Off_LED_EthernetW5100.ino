//*****************************************************************************
/// @file
/// @brief
///   Arduino SmartThings Ethernet W5100 On/Off with LED Example 
///
///   Revised by Dan Ogorchock on 2017-02-10 to work with new "SmartThings v2.0" Library
///
///   Notes: Arduino communicates with both the W5100 and SD card using the SPI bus (through the ICSP header). 
///         This is on digital pins 10, 11, 12, and 13 on the Uno and pins 50, 51, and 52 on the Mega. 
///         On both boards, pin 10 is used to select the W5100 and pin 4 for the SD card. 
///         These pins cannot be used for general I/O. On the Mega, the hardware SS pin, 53, 
///         is not used to select either the W5100 or the SD card, but it must be kept as an output 
///         or the SPI interface won't work.
///         See https://www.arduino.cc/en/Main/ArduinoEthernetShieldV1 for details on the W5100 Sield
///
///
//*****************************************************************************

#include <SmartThingsEthernetW5100.h>

//*****************************************************************************
// Pin Definitions    | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//                    V V V V V V V V V V V V V V V V V V V V V V V V V V V V V
//*****************************************************************************
// "RESERVED" pins for W5100 Ethernet Shield - best to avoid
#define PIN_4_RESERVED            4  //reserved by W5100 Shield on both UNO and MEGA
#define PIN_1O_RESERVED          10  //reserved by W5100 Shield on both UNO and MEGA
#define PIN_11_RESERVED          11  //reserved by W5100 Shield on UNO
#define PIN_12_RESERVED          12  //reserved by W5100 Shield on UNO
#define PIN_13_RESERVED          13  //reserved by W5100 Shield on UNO
#define PIN_50_RESERVED          50  //reserved by W5100 Shield on MEGA
#define PIN_51_RESERVED          51  //reserved by W5100 Shield on MEGA
#define PIN_52_RESERVED          52  //reserved by W5100 Shield on MEGA
#define PIN_53_RESERVED          53  //reserved by W5100 Shield on MEGA

#define PIN_LED 13  //Onboard LED

//*****************************************************************************
// Global Variables   | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//                    V V V V V V V V V V V V V V V V V V V V V V V V V V V V V
//*****************************************************************************
SmartThingsCallout_t messageCallout;    // call out function forward decalaration

//******************************************************************************************
//W5100 Ethernet Shield Information  CHANGE THIS INFORMATION ACCORDINGLY FOR YOUR NETWORK!
//****************************************************************************************** 
byte mac[] = {0x06,0x02,0x03,0x04,0x05,0x06}; //MAC address, leave first octet 0x06, change others to be unique //  <---You must edit this line!
IPAddress ip(192, 168, 1, 204);               //Arduino device IP Address                   //  <---You must edit this line!
IPAddress gateway(192, 168, 1, 1);            //router gateway                              //  <---You must edit this line!
IPAddress subnet(255, 255, 255, 0);           //LAN subnet mask                             //  <---You must edit this line!
IPAddress dnsserver(192, 168, 1, 1);          //DNS server                                  //  <---You must edit this line!
const unsigned int serverPort = 8090;         // port to run the http server on

// Smartthings hub information
IPAddress hubIp(192,168,1,149);               // smartthings hub ip                         //  <---You must edit this line!
const unsigned int hubPort = 39500;           // smartthings hub port

//Create a SmartThings Ethernet W5100 object
st::SmartThingsEthernetW5100 smartthing(mac, ip, gateway, subnet, dnsserver, serverPort, hubIp, hubPort, messageCallout);

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
  smartthing.send("on");        // send message to cloud
}

//*****************************************************************************
void off()
{
  stateLED = 0;                 // set state to 0 (off)
  digitalWrite(PIN_LED, LOW);   // turn LED off
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

