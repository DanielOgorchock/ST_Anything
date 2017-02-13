//******************************************************************************************
//  File: ST_Anything_Relays_ThingShield.ino
//  Authors: Dan G Ogorchock 
//
//  Summary:  This Arduino Sketch, along with the ST_Anything library and the revised SmartThings 
//            library, demonstrates the ability of one Arduino + SmartThings Shield to 
//            implement 16 Relays which can be mapped back to a SmartThings "Switches"
//
//            This version also includes support for 16 local pushbuttons that can be used to 
//            toggle the state of any relay (i.e. local control.)  Using these pushbuttons
//            will still result in the SmartThings cloud being updated via the ThingShield.
//            NOTE:  The pushbutton feature is entirely encapsulated within this sketch.   
//            None of the ST_Anything libraries were changed to implement this feature.
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
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//    2015-03-27  Dan Ogorchock  Modified for 16 Relay + 16 Pushbuttons Design
//    2015-03-31  Daniel O.      Memory optimizations utilizing progmem
//    2017-02-12  Dan Ogorchock  Revised to use the new SMartThings v2.0 library
//
//******************************************************************************************

//******************************************************************************************
// SmartThings Library for Arduino Shield
//******************************************************************************************
#include <SmartThingsThingShield.h>     //Library to provide API to the SmartThings Shield

//******************************************************************************************
// ST_Anything Library 
//******************************************************************************************
#include <Constants.h>       //Constants.h is designed to be modified by the end user to adjust behavior of the ST_Anything library
#include <Device.h>          //Generic Device Class, inherited by Sensor and Executor classes
#include <Sensor.h>          //Generic Sensor Class, typically provides data to ST Cloud (e.g. Temperature, Motion, etc...)
#include <Executor.h>        //Generic Executor Class, typically receives data from ST Cloud (e.g. Switch)
#include <Everything.h>      //Master Brain of ST_Anything library that ties everything together and performs ST Shield communications

#include <EX_Switch.h>       //Implements an Executor (EX) via a digital output to a relay

//******************************************************************************************
//Define which Arduino Pins will be used for each device
//  Notes: -Serial Communications Pins are defined in Constants.h (avoid pins 0,1,2,3
//          for inputs and output devices below as they may be used for communications)
//         -Always avoid Pin 6 as it is reserved by the SmartThings Shield
//
//******************************************************************************************
//"RESERVED" pins for SmartThings ThingShield - best to avoid
#define PIN_O_RESERVED               0  //reserved by ThingShield for Serial communications OR USB Serial Monitor
#define PIN_1_RESERVED               1  //reserved by ThingShield for Serial communications OR USB Serial Monitor
#define PIN_2_RESERVED               2  //reserved by ThingShield for Serial communications
#define PIN_3_RESERVED               3  //reserved by ThingShield for Serial communications
#define PIN_6_RESERVED               6  //reserved by ThingShield (possible future use?)


#define PIN_RELAY1   22
#define PIN_RELAY2   23
#define PIN_RELAY3   24
#define PIN_RELAY4   25
#define PIN_RELAY5   26
#define PIN_RELAY6   27
#define PIN_RELAY7   28
#define PIN_RELAY8   29
#define PIN_RELAY9   30
#define PIN_RELAY10  31
#define PIN_RELAY11  32
#define PIN_RELAY12  33
#define PIN_RELAY13  34
#define PIN_RELAY14  35
#define PIN_RELAY15  36
#define PIN_RELAY16  37

//---Begin Push Button declarations---
#define MAX_PUSHBUTTONS 16
#define MIN_DEBOUNCE_TIME 50 //push-buttons must be held for 50ms to prevent chattering input

#define PIN_BUTTON1   38
#define PIN_BUTTON2   39
#define PIN_BUTTON3   40
#define PIN_BUTTON4   41
#define PIN_BUTTON5   42
#define PIN_BUTTON6   43
#define PIN_BUTTON7   44
#define PIN_BUTTON8   45
#define PIN_BUTTON9   46
#define PIN_BUTTON10  47
#define PIN_BUTTON11  48
#define PIN_BUTTON12  49
#define PIN_BUTTON13  50
#define PIN_BUTTON14  51
#define PIN_BUTTON15  52
#define PIN_BUTTON16  53

byte nBtnIndex;      //Index Variable
bool nCurrentVal;     //temp variable
String strCommand; 
byte nBtnVals[MAX_PUSHBUTTONS][2];   //Array of current[0] and last[1] values of the pushbuttons
byte nBtnPins[MAX_PUSHBUTTONS] = {PIN_BUTTON1, PIN_BUTTON2, PIN_BUTTON3, PIN_BUTTON4,
                                 PIN_BUTTON5, PIN_BUTTON6, PIN_BUTTON7, PIN_BUTTON8,
                                 PIN_BUTTON9, PIN_BUTTON10, PIN_BUTTON11, PIN_BUTTON12,
                                 PIN_BUTTON13, PIN_BUTTON14, PIN_BUTTON15, PIN_BUTTON16};
unsigned long lngBtnLastMillis[MAX_PUSHBUTTONS]; //needed to properly debounce the pushbutton inputs
st::EX_Switch* swArray[MAX_PUSHBUTTONS]; //need an array of the executors so we can togle the correct one
//---End Push Button declarations--- 

//If using SoftwareSerial (e.g. Arduino UNO), must define pins for transmit and receive
#define pinRX 3
#define pinTX 2

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
  //           main comments in the corresponding header (.h) and program (.cpp) files.
  //         - The name assigned to each device (1st argument below) must match the Groovy
  //           DeviceType Tile name in your custom DeviceType code. 
  //******************************************************************************************
  //Polling Sensors
  
  //Interrupt Sensors 
  
  //Executors
      
  //EX_Switch arguments(name, pin, starting state,  invert logic) change last 2 args as needed for your application 
  static st::EX_Switch executor1(F("switch1"), PIN_RELAY1, LOW, false);
  static st::EX_Switch executor2(F("switch2"), PIN_RELAY2, LOW, false);
  static st::EX_Switch executor3(F("switch3"), PIN_RELAY3, LOW, false);
  static st::EX_Switch executor4(F("switch4"), PIN_RELAY4, LOW, false);
  static st::EX_Switch executor5(F("switch5"), PIN_RELAY5, LOW, false);
  static st::EX_Switch executor6(F("switch6"), PIN_RELAY6, LOW, false);
  static st::EX_Switch executor7(F("switch7"), PIN_RELAY7, LOW, false);
  static st::EX_Switch executor8(F("switch8"), PIN_RELAY8, LOW, false);
  static st::EX_Switch executor9(F("switch9"), PIN_RELAY9, LOW, false);
  static st::EX_Switch executor10(F("switch10"), PIN_RELAY10, LOW, false);
  static st::EX_Switch executor11(F("switch11"), PIN_RELAY11, LOW, false);
  static st::EX_Switch executor12(F("switch12"), PIN_RELAY12, LOW, false);
  static st::EX_Switch executor13(F("switch13"), PIN_RELAY13, LOW, false);
  static st::EX_Switch executor14(F("switch14"), PIN_RELAY14, LOW, false);
  static st::EX_Switch executor15(F("switch15"), PIN_RELAY15, LOW, false);
  static st::EX_Switch executor16(F("switch16"), PIN_RELAY16, LOW, false);

  //*****************************************************************************
  //  Configure debug print output from each main class 
  //  -Note: Set these to "false" if using Hardware Serial on pins 0 & 1
  //         to prevent communication conflicts with the ST Shield communications
  //*****************************************************************************
  st::Everything::debug=true;
  st::Executor::debug=true;
  st::Device::debug=true;
  
  //*****************************************************************************
  //Initialize the "Everything" Class
  //*****************************************************************************

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
  
  //*****************************************************************************
  //Add each executor to the "Everything" Class
  //*****************************************************************************
  st::Everything::addExecutor(&executor1);
  st::Everything::addExecutor(&executor2);
  st::Everything::addExecutor(&executor3);
  st::Everything::addExecutor(&executor4);
  st::Everything::addExecutor(&executor5);
  st::Everything::addExecutor(&executor6);
  st::Everything::addExecutor(&executor7);
  st::Everything::addExecutor(&executor8);
  st::Everything::addExecutor(&executor9);
  st::Everything::addExecutor(&executor10);
  st::Everything::addExecutor(&executor11);
  st::Everything::addExecutor(&executor12);
  st::Everything::addExecutor(&executor13);
  st::Everything::addExecutor(&executor14);
  st::Everything::addExecutor(&executor15);
  st::Everything::addExecutor(&executor16);

  //*****************************************************************************
  //Initialize each of the devices which were added to the Everything Class
  //*****************************************************************************
  st::Everything::initDevices();
  
  //*****************************************************************************
  //Add User Customized Setup Code Here (instead of modifying standard library files)
  //*****************************************************************************
  //---Begin Push Button initialization section---
  swArray[0]=&executor1;
  swArray[1]=&executor2;
  swArray[2]=&executor3;
  swArray[3]=&executor4;
  swArray[4]=&executor5;
  swArray[5]=&executor6;
  swArray[6]=&executor7;
  swArray[7]=&executor8;
  swArray[8]=&executor9;
  swArray[9]=&executor10;
  swArray[10]=&executor11;
  swArray[11]=&executor12;
  swArray[12]=&executor13;
  swArray[13]=&executor14;
  swArray[14]=&executor15;
  swArray[15]=&executor16;
    
  //Allocate strCommand buffer one time to prevent Heap Fragmentation.
  strCommand.reserve(20);

  //Configure input pins for hardwired pusbuttons AND read initial values
  for (nBtnIndex=0; nBtnIndex < MAX_PUSHBUTTONS; nBtnIndex++) {
    pinMode(nBtnPins[nBtnIndex], INPUT_PULLUP);
    nBtnVals[nBtnIndex][0] = digitalRead(nBtnPins[nBtnIndex]);   // read the input pin
    nBtnVals[nBtnIndex][1] = nBtnVals[nBtnIndex][0];
    lngBtnLastMillis[nBtnIndex] = 0;  //initialize times to zero
  }
 //---End Push Button initialization section---
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
  
  //*****************************************************************************
  //Add User Customized Loop Code Here (instead of modifying standard library files)
  //*****************************************************************************
  
  //---Begin Push Button execution section---
  //Loop through the pushbutton array
  for (nBtnIndex=0; nBtnIndex < MAX_PUSHBUTTONS; nBtnIndex++) 
  {
    nCurrentVal = digitalRead(nBtnPins[nBtnIndex]); // read the input pin   
    if (nCurrentVal != nBtnVals[nBtnIndex][1])      // only act if the button changed state
    {
      lngBtnLastMillis[nBtnIndex] = millis();  //keep track of when the button changed state
    }
    if ((millis() - lngBtnLastMillis[nBtnIndex] >= MIN_DEBOUNCE_TIME) && (nBtnVals[nBtnIndex][0] != nCurrentVal))
    {
      nBtnVals[nBtnIndex][0] = nCurrentVal; //keep current value for proper debounce logic
      if (nCurrentVal == LOW)    //only care if the button is pressed (change LOW to HIGH if logic reversed)
      {
        strCommand = swArray[nBtnIndex]->getName() + " " + (swArray[nBtnIndex]->getStatus()== HIGH?"off":"on");
        Serial.print(F("Pushbutton: "));
        Serial.println(strCommand);
        swArray[nBtnIndex]->beSmart(strCommand);  //Call the beSmart function of the proper executor object to either turn on or off the relay
        strCommand.remove(0); //clear the strCommand buffer
      }
    }
    nBtnVals[nBtnIndex][1] = nCurrentVal;  //keep last value for proper debounce logic
  }  
 //---End Push Button execution section---  
}
