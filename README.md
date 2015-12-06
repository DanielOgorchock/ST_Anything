History:
- v1.0 2015-01-05 Initial Release
- v1.1 2015-01-11 Added st::IS_DoorControl device class & ST_Anything_Doors example code (sketch + groovy)
- v1.2 2015-02-12 Added st::EX_RCSwitch device class & ST_Anything_RCSwitch example code (sketch and groovy), included DHT and RCSwitch libraries in the Arduino\libraries\ folder of the repo to simplify initial setup for users.
- v1.3 2015-03-28 Added Furnace Alarm, Temperatures, and Relays example code (sketches and groovy code)
- v1.4 2015-04-14 Memory Optimizations
- v1.5 2015-12-06 Added Alarm_Panel MEGA 2560 example, as well as adding Smoke Detector capability

ST_Anything
===========
Turn your Arduino into an AnyThing. ST_Anything is an Arduino library, sketch, and Device Type that works with your SmartThings ThingShield to create an all-in-one SmartThings device.


![screenshot](https://cloud.githubusercontent.com/assets/5206084/5608219/53df3d16-944a-11e4-9791-ce42f7334990.PNG)


This package currently implements the following SmartThings Device Capabilities:
- Alarm (Siren only currently) (HoneyWell Wave 2 Siren (http://amzn.com/B0006BCCAE) + relay)
- Configuration
- Illuminance Measurement (photo resistor)
- Motion Sensor (HC-SR501 Infrared PIR)
- Relative Humidity Measurement (DHT22)
- Switch (Sunfounder Relay - http://amzn.com/B00E0NTPP4)
- Temperature Measurement (DHT22 - requires DHT Library, see below)
- Water Sensor (http://amzn.com/B00HTSL7QC)
- Contact Sensor  (Magnetic Door Switch)
- Door Control (i.e. Garage Door Contact Sensor + Relay Output) - New! See 'ST_Anything_Doors' example
- RCSwitch Control (i.e. Radio Control Switch) - New! See 'ST_Anything_RCSwitch' example (Requires RCSwitch library, see below)
- Thermocouple Temperature Measurement (via the Adafruit MAX31855 library)
- Smoke Detector (as a simple digital input)

Note: Attempting to use all of these at once on an Arduino UNO is likely to result in running out of SRAM on the UNO (the UNO only has 2 kilobytes of RAM.)  Using an Arduino MEGA 2560 with 8 kilobytes of SRAM is recommended if you want to run everything at once.


## Overview
ST_Anything (original example) consists of four parts:
- The ST_Anything.ino Arduino sketch
- The ST_Anything Arduino library
- The SmartThings (ThingShield) library - A modified, more efficient version 
- The ST_Anything.groovy DeviceType

##ST_Anything Arduino Setup Instructions
- Download the ST_Anything repository.
- This folder structure should mirror that of your local Arduino directory. 
  - On Mac, it's located in `~/Documents/Arduino/`.
  - On Windows, it's located in `C:\My Documents\Arduino`.
- Look inside the `Arduino/Sketches` folder of the repo.
- Copy and paste the `ST_Anything` sketch folder into your local Arduino sketches directory. If you haven't created any sketches, you may not see the folder. In this case, feel free to create it.
- Look inside the `Arduino/libraries` folder of the repo.
- Copy and paste both the `ST_Anything` and `SmartThings`  (as well as all of the other new libraries from v1.2 and v1.3) folders into your local Arduino libraries directory. (Note: it may be wise to rename your existing 'SmartThings' library to prevent any overwriting if you have already downloaded the official release.)
- Download DHT library from https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTlib and copy dht.h, dht.cpp, and ReadMe.txt to your 'Arduino\libraries\DHT' folder. NOTE:  This library is now included in my github repo (v1.2)
- Download the RCSwitch library from http://code.google.com/p/rc-switch/downloads/list?can=3&q= and copy it to your 'Arduino\libraries\RCSwitch' folder. NOTE:  This library is now included in my github repo (v1.2)
- Open the ST_Anything.ino and see if it successfully compiles.
- WARNING:  If you are using an Arduino UNO, you will most likely need to comment out some of the devices in the sketch (both in the global variable declaration section as well as the setup() function) due to the UNO's limited 2 kilobytes of SRAM.  Failing to do so will most likely result in unpredictable behavior. The Arduino MEGA 2560 has 8k of SRAM and has four Hardware Serial ports (UARTs).  If you plan on using lots of devices, get the MEGA 2560.

##ST_Anything SmartThings Device Type Installation Instructions
- Join your Arduino/ThingShield to your hub using your phone's SmartThings App.  It will show up as a generic "Arduino ThingShield"
- Create an account and/or log into the SmartThings Developers Web IDE.
- Click on My Device Types from the navigation menu.
- Click on  + New SmartDevice button.
- Fill in the Name field as ST_Anything and click on the Create button. Don't worry about filling out everything else.
- Paste the code from the ST_Anything.groovy file from this repo.
- If you commented out any of the devices in the sketch, be sure to comment out the corresponding tiles & preferences in the ST_Anything.groovy code as well.
- Click on  Save  in the IDE.
- Click on  Publish -> For Me  in the IDE.
- Click on My Devices from navigation menu
- Select your "Arduino ThingShield" device from the list
- Click the Edit button at the bottom of the screen
- Change the Type to "ST_Anything"
- Click the Update button at the bottom of the screen
- On your phone, log out of SmartThings in the app, and then log back into SmartThings to refresh its settings
- Your Arduino Device Tile should now look like the image above in this ReadMe
- Be sure to go into the Preferences section to set the polling rates for the sensors.  These are sent to the Arduino if you press the Configure tile.  (Note:  Currently, these settings do not persist after an Arduino reboot.  I am hoping to figure out a method to have SmartThings send the Configure() command each time the Arduino starts up.  Gotta leave something for the future! :) )


##Updated SmartThings ThingShield Library
While developing the ST_Anything library and Arduino sketch, it was discovered that the Arduino UNO R3's 2 kilobytes of SRAM was quickly limiting the number of devices that could be hosted simultaneously.  Numerous optimizations were made to the ST_Anything library which resulted in significant savings.  Focus was then turned to the SmartThings ThingShield library.
 
Improvements to the SmartThings ThingShield library include:
- 100% backwards compatible for use with your existing Arduino code
- Performance improvements
- SRAM memory optimizations (~150 bytes saved)
- Elimination of unnecessary temporary dynamic memory allocations (255 bytes per send command)
- Elimination of unused variables and dead code paths
- The additon of a Hardware Serial communications constructor
- Support for the 3 additional hardware UARTS on the Arduino MEGA 2560

If you choose to use the complete ST_Anything package (i.e. ST_Anything.ino, ST_Anything library, and ST_Anything.groovy) the following choices are automatically made:
- If using an Arduino UNO - SoftwareSerial is used on pins 2/3
- If using an Arduino Leonardo - SoftwareSerial is used on pins 2/10 (add jumper from pin 10 to pin 2)
- If using an Arduino MEGA, Hardware Serial is used on pins 14/15 (add jumpers from Pin14 to Pin2 and another from Pin15 to Pin3)

.
.
.

###WARNING - Geeky Material Ahead!!!

.
.
.

If you want to use the new SmartThings libary with your existing sketches (or to just learn more about how all this stuff works, keep reading...) 

The Arduino UNO should typically use the SoftwareSerial library Constructor since the UNO has only one Hardware UART port ("Serial") which is used by the USB port for programming and debugging.
To use SoftwareSerial:
- Use the original SoftwareSerial constructor passing in pinRX=3 and pinTX=2
  - SmartThings(uint8_t pinRX, uint8_t pinTX, SmartThingsCallout_t *callout) call.
- Make sure the ThingShield's switch in the "D2/D3" position
- Be certain to not use Pins 2 & 3 in your Arduino sketch for I/O since they are electrically connected to the ThingShield. Pin6 is also reserved by the ThingShield. Best to avoid using it. 

The Arduino Leonardo and Mega can use SoftwareSerial BUT cannot use Pin3 for Rx since that pin does not support interrupts on these boards.
To use SoftwareSerial:
- Use Pin 10 for Rx and add a wire jumper from Pin10 to Pin3. Use Pin 2 for Tx as usual
- Use the original SoftwareSerial constructor passing in pinRX=10 and pinTX=2 
  - SmartThings(uint8_t pinRX, uint8_t pinTX, SmartThingsCallout_t *callout);
- Make sure the ThingShield's switch in the "D2/D3" position
- Be certain to not use Pins 2 & 3 in your Arduino sketch for I/O since they are electrically connected to the ThingShield. Pin6 is also reserved by the ThingShield. Best to avoid using it.

The Arduino UNO, Leonardo, and MEGA can use the Hardware "Serial" (pins 0,1) if desired, but USB programming and debug will be troublesome. 
To use Hardware Serial:
- Use the new Hardware Serial constructor passing in the Arduino's pin 0/1 UART (i.e "HW_SERIAL")
  - SmartThings(SmartThingsSerialType_t hwSerialPort, SmartThingsCallout_t *callout);
  - Note: SmartThingsSerialType_t is a new enum declared in SmartThings.h.  For the pin 0/1 UART, pass in "HW_SERIAL"
- Download your sketch from the IDE with the ThingShield's switch in the "D2/D3" position
- After the download is complete, move the switch to the "D0/D1" position and press RESET to allow the program to restart
- You must suppress any and all "Serial.begin(), Serial.print(), Serial.println(), Serial.write(), Serial.read(), Serial.end(), Serial..." commands from your code when using Hardware Serial on pins 0/1 to avoid conflicts with the ThingShield's communication with the SmartThings library.
- Pin6 is also reserved by the ThingShield. Best to avoid using it.
 
The Arduino MEGA should use the new Hardware Serial Constructor since it has 4 UARTs. 
To use Hardware Serial on "Serial3":
- The "Serial3" port uses pins 14(Tx) and 15(Rx).  Wire a jumper Pin14 to Pin2 and another from Pin15 to Pin3.
- Use the new Hardware Serial constructor passing in the Arduino's pin 14/15 UART (i.e "HW_SERIAL3")
  - SmartThings(SmartThingsSerialType_t hwSerialPort, SmartThingsCallout_t *callout);
  - Note: SmartThingsSerialType_t is a new enum declared in SmartThings.h.  For the pin 14/15 UART, pass in "HW_SERIAL3"
- Make sure the ThingShield's switch in the "D2/D3" position 
- Be certain to not use Pins 2 & 3 in your Arduino sketch for I/O since they are electrically connected to the ThingShield. Pin6 is also reserved by the ThingShield. Best to avoid using it.

Additional Information:
The SoftwareSerial library has the following known limitations: 
- If using multiple software serial ports, only one can receive data at a time.
- Not all pins on the Mega and Mega 2560 support change interrupts, so only the following can be used for RX: 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8(62), A9(63), A10(64), A11(65), A12(66), A13(67), A14(68), A15(69).
- Not all pins on the Leonardo and Micro support change interrupts, so only the following can be used for RX : 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

For more details on the methods used in ST_Anything to measure and optimize the amount of free RAM, please refer to https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory and  https://learn.adafruit.com/memories-of-an-arduino/optimizing-sram.  This is a great site for any Arduino programmer.

More instructions coming soon:

For now, reference the header files of the ST_Anything library for explanation of specific classes. 

Look at the documentation in the 'ST_Anything.ino' file for explanation of general use of the library.  
