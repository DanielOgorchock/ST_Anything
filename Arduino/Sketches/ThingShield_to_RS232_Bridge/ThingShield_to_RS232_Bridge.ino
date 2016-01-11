//*****************************************************************************
//  Arduino SmartThings Shield / RS232 Shield Bridge example
//
//  Written by Dan Ogorchock on 2015-10-16
// 
//*****************************************************************************
#include <SmartThings.h>  //Must use version from ST_Anything GitHub Repository for HW Serial Support

//*****************************************************************************
// Note:  Pins 2/3 are reserved for use by the SmartThings ThingShield
//        Pins 4/5 are reserved for use by the RS2323 Shield
//*****************************************************************************
SmartThingsCallout_t messageCallout;    // call out function forward decalaration
SmartThings smartthing(HW_SERIAL3, messageCallout);  // Hardware Serial constructor

void setup()
{
    Serial.begin(9600);                         //setup console serial with a baud rate of 9600
    Serial.println(F("HW 'Serial' setup.."));  
    Serial2.begin(9600);                        //setup RS232 serial with a baud rate of 9600
    Serial2.println(F("HW 'RS232' Setup..."));
}

void loop()
{
  //Run the ThingShield
  smartthing.run();

  //Read the RS232 port and forward data to ThingShield
  if (Serial2.available()>0)
  {
    String rs232String = Serial2.readString();
    rs232String.trim();
    Serial.print(F("RS232 Data = "));
    Serial.println(rs232String);
    smartthing.send(rs232String);
  }
}

void messageCallout(String message)
{
    message.trim();           //Ignore the periodic ' ' sent 
    if (message.length() > 0) //from the ThingShield
    {
      Serial.print(F("ThingShield data = "));
      Serial.println(message);
      //Forward ThingShield message to the RS232 port
      Serial2.println(message);
    
      if (message.equals(F("hello"))) //Sample Code for visual effect :)
      {
        hello();
      }
    }
}

void hello()
{
  Serial.println("brobasaur");
  smartthing.send("colors!");
  smartthing.shieldSetLED(1, 0, 0);
  delay(200);
  smartthing.shieldSetLED(0, 1, 0);
  delay(200);
  smartthing.shieldSetLED(0, 0, 1);
  delay(200);
  smartthing.shieldSetLED(1, 1, 0);
  delay(200);
  smartthing.shieldSetLED(1, 1, 1);
  delay(200);
  smartthing.shieldSetLED(1, 0, 1);
  delay(200);
  smartthing.shieldSetLED(0, 1, 1);
  delay(200);
  smartthing.shieldSetLED(3, 2, 1);
  delay(200);
  smartthing.shieldSetLED(1, 2, 3);
  delay(200);
  smartthing.shieldSetLED(2, 2, 4);
  delay(200);
  smartthing.shieldSetLED(4, 3, 1);
     delay(200);
  smartthing.shieldSetLED(0, 0, 0);
  smartthing.send(F("fancy"));
}

