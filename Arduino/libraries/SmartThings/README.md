History:
- v2.0 2017-02-11 Initial Release
- v2.1 2017-02-20 Added support for Arduino + ESP-01 hardware combination
- v2.2 2017-04-26 Added additional optional argument to each constructor which can be queried to determine maximum data transfer rate for each type of communication.  This field is not used directly by the SmartThings library.  It is used by ST_Anything's Everything Class.
- v2.3 2017-05-25 Added support for the W5500 Ethernet Shield.  This required the library to be split into multiple library folders to avoid duplicate class defintions at compile/link time.
- v2.4 2017-08-16 Added support for the ESP32
- v2.5 2017-01-06 Added Arduino OTA support for the ESP8266 boards

SmartThings v2.x
================
This is a new version of the old SmartThings Arduino Library created by SmartThings for use with their Zigbee-based ThingShield.  Recently, SmartThings has decided to no longer produce nor support the ThingShield.  In an attempt to provide the Maker Community with alternatives, I decided to re-architect the C++ Class hierarchy of the SmartThings library to include continued support for the old ThingShield, as well as adding new support for the Arduino + Ethernet W5100 shield, Arduino + ESP-01 WiFi board, and the standalone NodeMCU ESP8266 and ESP-01 boards.  This allows users five options for connecting their DIY projects to SmartThings.

This library currently implements the following C++ Classes and associated constructors:
-st::SmartThings (base class, not to be used in your sketch!)
  -st::SmartThingsThingShield (use this class "#include <SmartThingsThingShield.h" if connecting an Arduino UNO/MEGA using a ThingShield)
  -st::SmartThingsEthernet (base class, not to be used in your sketch!)
     -st::SmartThingsEthernetW5100 (use this class "#include <SmartThingsEthernetW5100.h" if connecting an Arduino UNO/MEGA using a W5100 Ethernet shield)
     -st::SmartThingsEthernetW5500 (use this class "#include <SmartThingsEthernetW5500.h" if connecting an Arduino UNO/MEGA using a W5500 Ethernet shield)
	 -st::SmartThingsWiFiEsp (use this class "#include <SmartThingsWiFiEsp.h>" if using an Arduino attached to a ESP-01 (AT Firmware) for WiFi)
     -st::SmartThingsESP8266WiFi (use this class "#include <SmartThingsESP8266.h" if using a standalone NodeMCU ESP8266 or ESP-01)
     -st::SmartThingsESP32WiFi (use this class "#include <SmartThingsESP32.h" if using a standalone ESP32)

All five of the usable classes implement the following methods:
- send(String) used to send an ASCII string to the SmartThings Device Handler
- run() used in the sketch's loop() routine to execute background processing for the library to check for incoming data from the SmartThings Device Handler
- init() used in the sketch's setup() routine to establish the initial connection to the ST Hub 
	 
## Overview
SmartThings library consists of:
- Arduino SmartThings*.h and .cpp files to implement the class hierarchy show above.
- In the `examples` folder, sample sketches for using the Arduino/ThingShield, Arduino/EthernetW5100, Arduino/ESP-01, and NodeMCU ESP8266.
  - SmartThings_On_Off_LED_ThingShield
  - SmartThings_On_Off_LED_EthernetW5100
  - SmartThings_On_Off_LED_EthernetW5500
  - SmartThings_On_Off_LED_WiFiEsp
  - SmartThings_On_Off_LED_ESP8266WiFi (no Arduino!)
  - SmartThings_On_Off_LED_ESP01WiFi (no Arduino!)
- In the `extras` folder, two SmartThings Groovy Device Handlers to be used with the example sketches above
  -On_Off_LED_ThingShield.device.groovy for use with a ThingShield
  -On_Off_LED_Ethernet.device.groovy for use with
    - Arduino/W5100 combo
	- Arduino/W5500 combo
	- Arduino/ESP-01 combo (not reliable currently)
	- NodeMCU ESP8266 board (standalone)
	- ESP-01 module (standalone)


##SmartThings Library Installation Instructions
- Download the SmartThings library and copy it to your your Arduino "libraries" folder
  - On Windows, it's located in `C:\My Documents\Arduino\libraries`
  - On Mac, it's located in `~/Documents/Arduino/libraries`
  - Restart the Arduino IDE so it will scan the new library
  - Assumption: If you're using an ESP8266 or ESP32 based board, you should already have added support for it to the IDE (Google it if needed.)

##Pre-Requisites for using Ethernet based connectivity (Arduino/W5100, Arduino/ESP-01, NodeMCU ESP8266, ESP-01)
- Your SmartThings HUB must have a Static TCP/IP Address assigned via your router's DHCP Server.  Since this procedure varies by router model, Google it!
- You'll need to identify a static TCP/IP address for your Arduino/W5100, Arduino/ESP-01, NodeMCU ESP8266, or ESP-01, as you'll need this later when setting up the sketch.  Choose an unused IP address outside of the range your router's DHCP server uses.
  
##Arduino IDE Instructions
- Open the Arduino IDE and select File->Examples->SmartThings
  - Select the example sketch that matches your hardware
  - Select File->Save As and select your "Sketches" folder, and click "Save"
  - If using Arduino/W5100, Arduino/ESP-01 or ESP8266, find the lines of the Sketch where it says "<---You must edit this line!"
	- The Arduino must be assigned a static TCP/IP address, Gateway, DNS, Subnet Mask, MAC Address (W5100 only), SSID + Password (WiFiEsp + ESP8266 only)
	- *** NOTE:  If using the W5100 Shield, YOU MUST ASSIGN IT A UNIQUE MAC ADDRESS in the sketch! Please leave the first octet in the MAC Address '06' as certain MAC addresses are UNICAST while others are MULTICAST.  Your MAC must be UNICAST and be a 'Locally Administered Address' Please see https://en.wikipedia.org/wiki/MAC_address#Address_details for more information ***
	- *** NOTE:  If using the W5500 Shield, YOU MUST ASSIGN IT THE MAC ADDRESS that came with the shield in the sketch!
 	- Note:  If using an ESP-01 with an Arduino, the example assumes you're using an Arduino MEGA 2560.  Attach the ESP-01 to Hardware Serial "Serial1"  *** THE ARDUINO + ESP01 is currently not very stable!
  - Your IDE Serial Monitor Window should be set to 9600 baud
  - With the Serial Monitor windows open, load your sketch and watch the output
    - If using an Arduino/ESP-01, NodeMCU ESP8266 board, ESP-01, or ESP32 the MAC Address will be printed out in the serial monitor window.  Write this down as you will need it to configure the Device using your ST App on your phone. (Note: MAC Address must later be entered with no delimeters in the form of "06AB23CD45EF" (without quotes!))
  
##SmartThings IDE Device Handler Installation Instructions
- Create an account and/or log into the SmartThings Developers Web IDE.

Arduino/ThingShield
- If using a ThingShield, join it to the SmartThings hub by using the ST App on your phone - Add new device
- Install the ThingShield example Device Handler
  - Click on "My Device Handlers" from the navigation menu.
  - Click on  "+ New Device Handler" button.
  - Select the "From Code" Tab near the top of the page
  - Paste the code from the `On_Off_LED_ThingShield.device.groovy` file from the `..Arduino\libraries\SmartThings\extras` folder
  - Click on "Create" near the bottom of the page.
  - Click on  "Save"  in the IDE.
  - Click on  "Publish" -> "For Me"  in the IDE.
  - Click on "My Devices" from navigation menu
  - Select your "Arduino ThingShield" device from the list
  - Click the Edit button at the bottom of the screen
  - Change the Type to "On/Off ThingShield"
  - Click the Update button at the bottom of the screen
  - On your phone, you should now see a device that has simple On/Off tile

Ethernet Arduino/W5100, Arduino/W5500, Arduino/ESP-01, NodeMCU ESP8266, ESP-01, or ESP32   
- Install the Ethernet example Device Handler
  - Click on  "+ New Device Handler" button.
  - Select the "From Code" Tab near the top of the page
  - Paste the code from the `On_Off_LED_Ethernet.device.groovy` file from the `..Arduino\libraries\SmartThings\extras` folder
  - Click on "Create" near the bottom of the page.
  - Click on  "Save"  in the IDE.
  - Click on  "Publish" -> "For Me"  in the IDE.
  - Click on "My Devices" from navigation menu
  - Click on the "+ New Device" button
  - Enter a "Name" (whatever you like)
  - Enter a "Label" (whatever you like)
  - Change the Type to "On/Off Ethernet"
  - Change "Version" to "Published"
  - Select your "Location"
  - Select your "Hub"
  - Click "Create"
  - On your phone, you should now see a device that has simple "On/Off" tile and a "Configure" tile
  - One your phone, after selecting the new device, click on the settings icon (small gear in top right corner)
    - Enter your Arduino or ESP8266 device's TCP/IP Address, Port, and MAC Address (you should already know these from when you configured your sketch)
    - Note: MAC Address should be entered with no delimeters in the form of "06AB23CD45EF" (no quotes!)
    - Click "Done"
    - CLick the "Configure" tile to make sure the 'networkDeviceID' for your new ST Device is set to the MAC address you just entered

  - Note:  If you're trying to use a ESP-01 WiFi board with an Arduino MEGA, please make sure the ESP-01 is running the "AT Firmware" and figure out the baudrate ahead of time.  Please use the examples in the WiFiEsp library to get the ESP-01 wired and working on "Serial1" before attempting to use this library.  If you need further help on this, please Google "Arduino MEGA ESP-01" and I am sure you will find some good guides.  Also, please note that the Arduino boards really are not rated to power the ESP-01 from the Arduino's 3.3v pin.  I had to use an external 3.3v power supply for the ESP-01 to get everything working reliably.  Just remember to tie the 3.3v powersupply GND to the Arduino GND.  Do NOT connect the +3.3v power supply to the Arduino's 3.3v pin.  This would be bad!  You may also want to "level shift" the Arduino's 5v Tx pin down to 3.3v before connecting it to the Rx pin on the ESP-01.  Some folks online say it is a must, others say the ESP-01 input is 5v tolerant.  YMMV.  

.
.
.

###WARNING - Geeky Material Ahead!!!

.
.
.

The following applies only to using the ThingShield!  

If you want to use the new SmartThings v2.x libary with your existing ThingShield sketches (or to just learn more about how all this stuff works, keep reading...) 

Remember to "#include <SmartThingsThingShield.h>" to use the new v2.x SmartThings library with the ThingShield.

The Arduino UNO uses the SoftwareSerial library constructor since the UNO has only one Hardware UART port ("Serial") which is used by the USB port for programming and debugging.
Arduino UNO R3 SoftwareSerial:
- Use the NEW SoftwareSerial constructor passing in pinRX=3 and pinTX=2
  - st::SmartThingsThingShield(uint8_t pinRX, uint8_t pinTX, SmartThingsCallout_t *callout) call.
- Make sure the ThingShield's switch in the "D2/D3" position
- Be certain to not use Pins 2 & 3 in your Arduino sketch for I/O since they are electrically connected to the ThingShield. Pin6 is also reserved by the ThingShield. Best to avoid using it. Also, pins 0 & 1 are used for USB communications, so do not use them if possible.

The new v2.x SmartThings ThingShield library automatically disables support for Software Serial if using a Leonardo or MEGA based Arduino board.

The Arduino MEGA 2560 must use HardwareSerial "Serial1, Serial2, or Serial3" for communications with the ThingShield. 
MEGA 2560 Hardware Serial:
- Use the new Hardware Serial constructor passing in a pointer to a Hardware Serial device (&Serial1, &Serial2, &Serial3)
  - definition st::SmartThingsThingShield(HardwareSerial* serial, SmartThingsCallout_t *callout);
  - sample st::SmartThingsThingShield(&Serial3, callout);
- Make sure the ThingShield's switch in the "D2/D3" position 
- Be certain to not use Pins 2 & 3 in your Arduino sketch for I/O since they are electrically connected to the ThingShield. Pin6 is also reserved by the ThingShield. Best to avoid using it.
- On the MEGA, Serial1 uses pins 18/19, Serial2 uses pins 16/17, and Serial3 uses pins 14/15
- You will need to wire the MEGA's HardwareSerial port's pins to pins 2 & 3 on the ThingShield.  For example, using Serial3, wire Pin 2 to Pin 14 AND Pin3 to Pin 15.

The Arduino Leonardo must use HardwareSerial "Serial1" for communications with the ThingShield. 
Leonardo Hardware Serial:
- Use the new Hardware Serial constructor passing in a pointer to a Hardware Serial device (&Serial1)
  - definition st::SmartThingsThingShield(HardwareSerial* serial, SmartThingsCallout_t *callout);
  - sample st::SmartThingsThingShield(&Serial1, callout);
- Make sure the ThingShield's switch in the "D0/D1" position
- Be certain to not use Pins 0 & 1 in your Arduino sketch for I/O since they are electrically connected to the ThingShield. Pin6 is also reserved by the ThingShield. Best to avoid using it.



