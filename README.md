ST_Anything v2.9.2
================

**WARNING** - You must use the SmartThings Classic App with ST_Anything. 

**WARNING** - If using an ESP8266, make sure you are using v2.3 of the Arduino ESP8266 Board manager (v2.4/2.4.1 introduced a memory leak causing ST_Anythign to crash after an hour or two.) 

**Recent History:**
- v2.9.2 - Added support for I2C devices - AM2320(temp, humid), TSL2561(lux), MAX44009(lux), BH1750(lux), BME280(temp,humid,pressure), BMP280(temp, pressure), and improved TCS34725(color lux).  Added ST_Anything_I2C_ESP8266WiFi.ino example sketch to demonstrate I2C sensors.

- v2.9.1 - Added Servo Motor support, tweaked all device handlers for simplification and supportability, numerous small enhancements and bug fixes over the past few months (view Github History for details)


## Architecture Flow Chart

![screenshot](https://cloud.githubusercontent.com/assets/5206084/25318949/d00b08b4-2865-11e7-82f6-9a355bb9082b.PNG)

Note: The ST_Anything v1.6 release was created on 2017-02-11 to make sure everyone can still get back to the original ThingShield-only code if necessary.  
Note: If you want the old ST_Anything v2.2 code, please pull it by the v2.2 release number and follow the old v2.2 ReadMe 
Note: ST_Anything v2.9 was built using the Arduino IDE v1.8.5.  Please make sure to upgrade your IDE.

Turn your Arduino UNO/MEGA/MKR1000, NodeMCU ESP8266, or ESP32 into a Anything you can imagine! ST_Anything is an Arduino library, sketch, and Device Handlers that works with your hardware to create an all-in-one SmartThings device. 
- Arduino with SmartThings ThingShield
- Arduino with W5100 Ethernet shield
- Arduino with W5500 Ethernet shield
- Arduino with WiFi101 Shield (or Adafruit ATWINC1500 module)
- Arduino MKR1000 (has integrated WiFi101)
- Arduino with ESP-01 for WiFi
- Standalone NodeMCU v1.0 ESP8266-12e
- Standalone ESP-01 (or really any ESP8266 based board)
- Standalone ESP32 board

v2.0 Note:  There are some significant changes as compared to the old v1.x platform.  A signiciant rewrite of the "SmartThings" Arduino library was completed to incorporate Ethernet communications support.  To use ST_Anything v2.x, you must also use all of the other supporting libaries found in this GitHub repository.  Included is a the new SmartThings v2.x Arduino library which can be used standalone (examples are included in the library), or in conjunction with the ST_Anything library.
v2.6 Note:  Version 2.6 builds upon the changes in v2.x to incorporate SmartThings new Composite Device Handler (DH).  This new functionality allows one Parent DH to create many Child Devices (using Child DHs).  This allows more than one of each SmartThings capability per Arduino.  Previously, this was only possible through the use of a Multiplexer SmartApp and virtual devices.  The only manual device that has to be create within the ST IDE is the Parent.  The ST_Anything Parent DH has been written to automagically create Child Devices that exist, or are added to, the Arduino ST_Anything sketch.

THIS DOCUMENT IS A WORK IN PROGRESS!  The essential code is all here and has been tested.  Documentation is still lacking somewhat, so feel free to submit a pull request to improve this ReadMe as you try to get things working.

New v2.6+ Parent / Child Devices 
![screenshot](https://cloud.githubusercontent.com/assets/5206084/25319004/8b6ab50a-2866-11e7-9f47-6f2b4863311a.PNG)


This package currently implements the following SmartThings Device Capabilities:
- Alarm (Siren and Strobe both supported if desired, using 1 or 2 digital outputs to relay(s))
- Contact Sensor  (Magnetic Door/Window Switch such as http://a.co/6iwkHwG)
- Illuminance Measurement (using an analog input + photo resistor such as http://a.co/4zhbgEH)
- Motion Sensor (using a HC-SR501 Infrared PIR, for example. Can also be any motion detector that outputs a digital signal)
- Switch (simple digital output connected to a relay such Sunfounder Relay - http://amzn.com/B00E0NTPP4)
- Relative Humidity Measurement (DHT22, DHT11, AM2320)
- Temperature Measurement (DHT22 - requires Rob Tillaart's DHT 0.1.13 Library, included in this repo)
- Temperature Measurement (using Dallas Semiconductor DS18B20's, requires OneWire and DallasTemperature libraries included in this repo)
- Temperature Measurement (via the Adafruit MAX31855 Thermocouple library, included in this repo)
- Temperature Measurement (using AM2320 sensor) (also does humidity)
- Water Sensor (using an analog input a a simple moisture sesnsor such as http://amzn.com/B00HTSL7QC or http://a.co/eZNTyIH)
- Door Control / Garage Door Control (i.e. Garage Door Contact Sensor + Relay Output) - See 'ST_Anything_Doors' example
- Smoke Detector (as a simple digital input)
- Smoke Detector (using the MQ2 sensor)
- Relay Switch (implemented as a timed relay, that turns off locally - i.e. no ST cloud command necessary to turn off)
- Button (both pushed and held events are supported)
- Switch + Color Control (RGB) - used for analog RGB LED's
- Switch + Color Control (RGBW) - used for analog RGBW LED's
- Switch (using RCSwitch (i.e. Radio Control Switch) - See 'ST_Anything_RCSwitch' example (Requires RCSwitch library, included in this repo)
- Servo Motor (using Capability Switch Level (0-99) to map to Motor Angles (0-180) - See the 'ST_Anything_Servos...' example sketch
- Illuminance Measurement (using a TSL2561 I2C sensor)
- Illuminance Measurement (using a MAX44009 I2C sensor)
- Illuminance Measurement (using a BH1750 I2C sensor)
- Illuminance Measurement (using a Color TCS34725 I2C sensor)
- Temperature Measurement (using a AM2320 I2C sensor)
- Relative Humidity  Measurement (using a AM2320 I2C sensor)
- Temperature Measurement (using a BME280 I2C sensor)
- Relative Humidity  Measurement (using a BME280 I2C sensor)
- Pressure Measurement (using a BME280 I2C sensor)
- Temperature Measurement (using a BMP280 I2C sensor)
- Pressure Measurement (using a BMP280 I2C sensor)

Note: Attempting to use all of these at once on an Arduino UNO R3 is likely to result in running out of SRAM on the UNO (the UNO only has 2 kilobytes of RAM.)  Using an Arduino MEGA 2560 with 8 kilobytes of SRAM is recommended if you want to run everything at once.  The ESP8266, ESP32, and MKR1000 platforms also have a lot more memory available.


## Overview
ST_Anything consists of four main parts:
- The ST_Anything example sketches 
  - ST_Anything_Multiples_EthernetW5100.ino - Arduino UNO/MEGA + W5100 Ethernet Shield
  - ST_Anything_Multiples_EthernetW5500.ino - Arduino UNO/MEGA + W5500 Ethernet Shield
  - ST_Anything_Multiples_MEGAWiFiEsp.ino - Arduino MEGA + ESP-01 WiFi module with "AT Firmware"
  - ST_Anything_Multiples_WiFi101.ino - Arduino MEGA + WiFi101 Shield (or Adafruit ATWINC1500 module)
  - ST_Anything_Multiples_MKR1000.ino - Arduino MKR1000 (has onboard WiFi101 chip)
  - ST_Anything_Multiples_ESP8266WiFi.ino - NodeMCU v1.0 ESP8266-12e development board (no Arduino!)
  - ST_Anything_Multiples_ESP01WiFi.ino - ESP-01 (ESP8266-01) module (no Arduino!)
  - ST_Anything_Multiples_ThingShield.ino - Arduino UNO/MEGA + ST ThingShield
  - ST_Anything_Multiples_ESP32WiFi.ino - ESP32 development board (no Arduino!)
  - ST_Anything_AlarmPanel_ESP8266WiFi.ino - NodeMCU v1.0 ESP8266-12e development board (no Arduino!)
  - + a few more based of user requests
- The ST_Anything Arduino libraries + required 3rd party libraries
- The SmartThings libraries - A modified, more efficient version, now with added support for LAN-to-Hub based communications too! 
- The SmartThings Parent and Child Device Handlers that support sketches above.
  - parent-st-anything-ethernet.groovy (LAN-to-Hub, Arduino/W5100/W5500, Arduino/ESP-01, NodeMCU ESP8266-12e, ESP-01, ESP32, MKR1000)
  - parent-st-anything-thingshield.groovy (Thingshield-to-Hub)
  - child-xxxxxx.groovy 
    - currently 18 child device handlers are available!

## Pre-Requisites for using LAN-to-HUB Ethernet connectivity (Arduino/W5100, Arduino/ESP-01, NodeMCU ESP8266, ESP-01, ESP32)
- Your SmartThings HUB must have a Static TCP/IP Address assigned via your router's DHCP Server. Since this procedure varies by router model, Please Google it!
- You'll need to identify a static TCP/IP address for your Arduino/W5100, Arduino/ESP-01, NodeMCU ESP8266, or ESP-01, ESP32 as you'll need this later when setting up the sketch. Choose an unused IP address outside of the range your router's DHCP server uses, but in the same subnet.

## Arduino IDE Setup Instructions 
- Download the ST_Anything repository, focusing on the ST_Anything/Arduino/ folders
- This folder structure should mirror that of your local User Arduino directory. 
  - On Mac, it's located in `~/Documents/Arduino/`.
  - On Windows, it's located in `C:\Users\yourusername\Documents\Arduino`.
- Look inside the `Arduino/Sketches` folder of the repo.
- Copy and paste all of the `ST_Anything_...` sketch folders into your local Arduino sketches directory. If you haven't created any sketches, you may not see the folder. In this case, feel free to create it.
- Look inside the `Arduino/libraries` folder of the repo.
- Copy and paste both the `ST_Anything...` and `SmartThings...` folders (as well as ALL of the other library folders) into your local Arduino libraries directory. 
- Open one of the ST_Anything_Multiples_xxxxx.ino sketches for the hardware you're using and see if it successfully compiles.
  - Make sure you select the correct model of board you are compiling for. 
  - If building for a standalone ESP8266 board, make sure you have configured the Arduino IDE to include support for these boards.  Follow the guide at https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/installing-the-esp8266-arduino-addon
  
**NOTE: The v2.4/v2.4.1 ESP8266 Board support package for the Arduino IDE has introduced a nasty memory leak. Please be sure to revert back to v2.3 to prevent your ESP8266 from crashing every few hours. This is an issue that the Arduino ESP8266 developers are aware of and I believe will be fixed when they release the next version. (here are the details https://community.hubitat.com/clicks/track?url=https%3A%2F%2Fgithub.com%2Fesp8266%2FArduino%2Fissues%2F4497&post_id=1971&topic_id=399)**
  
  - If building for a standalone ESP32 board, make sure you have configured the Arduino IDE to include support for these boards.  Follow the guide at https://github.com/espressif/arduino-esp32/blob/master/README.md
- If using a LAN-to-Hub (WiFi or Cat5 Ethernet) based device
  -Find the lines of the Sketch where it says "<---You must edit this line!"
    - You must ensure your ST hub's LAN IP address does not change.  Use your router's static DHCP assignment feature to make sure your ST hub always gets the same IP Address!  Enter that address in the corresponding line in the sketch.
    - The Arduino must be assigned a static TCP/IP address, Gateway, DNS, Subnet Mask, MAC Address(W5100/W5500 only), SSID+Password(ESP8266,ESP01,ESP32,WiFi101,MKR1000)
    - *** NOTE: If using the W5100 Shield, YOU MUST ASSIGN IT A UNIQUE MAC ADDRESS in the sketch! Please leave the first octet in the MAC Address '06' as certain MAC addresses are UNICAST while others are MULTICAST. Your MAC must be UNICAST and be a 'Locally Administered Address' Please see https://en.wikipedia.org/wiki/MAC_address#Address_details for more information ***
    - *** NOTE: If using the W5500 Shield, YOU MUST ASSIGN IT A UNIQUE MAC ADDRESS in the sketch! Use the one packaged with the W5500 shield.
    - Note: If using an ESP-01 for WiFi-connecticity (i.e. as a WiFi dongle/shield) with an Arduino, the example assumes you're using an Arduino MEGA 2560. Attach the ESP-01 to Hardware Serial "Serial1" and make sure your ESP01 has current 'AT Firmware' installed.
  - Your IDE Serial Monitor Window must be set to 115200 baud  **** Please note recent change to 115200 ****
  - With the Serial Monitor window open, load your sketch and watch the output
  - The MAC Address will be printed out in the serial monitor window. Write this down as you will need it to configure the Device using your ST App on your phone. (Note: MAC Address must later be entered with no delimeters (i.e. NO COLONS!) in the form of "06AB23CD45EF" (without quotes!))

WARNING:  If you are using an Arduino UNO, you may need to comment out some of the devices in the sketch (both in the global variable declaration section as well as the setup() function) due to the UNO's limited 2 kilobytes of SRAM.  Failing to do so will most likely result in unpredictable behavior. The Arduino MEGA 2560 has 8k of SRAM and has four Hardware Serial ports (UARTs).  If you plan on using many devices, get an Arduino MEGA 2560, a MKR1000, a NodeMCU v1.0 ESP8266-12e, or a ESP32 board.

## SmartThings IDE Setup Instructions
- Create an account and/or log into the SmartThings Developers Web IDE.
- Click on "My Device Handlers" from the navigation menu.
- Click on "Settings" from the menu and add my GitHub Repository to your account
  - Owner:  DanielOgorchock
  - Name:   ST_Anything
  - Branch: master
- Click on "Update From Repo" from the menu
- Select "ST_Anything (master)" from the list
- Select all of the Parent and Child Device Handlers
- Check the "Publish" check box and click "Execute Update"
- You should now have all of the necessary Device Handlers added to your account

Note: If desired, you can still create all of the Device Handlers manually by copying and pasting code from the GitHub repository files into your ST IDE.  Trust me, the Github integration in SmartThings is so much easier!  And, you will know when new versions of the DHs are available based on the color of each DH in your list of Device Handlers in the IDE.

## SmartThings Device Handler Instructions - FOR USE WITH LAN-to-HUB WiFi/Cat5 Ethernet Devices
- Click on My Devices from navigation menu
- Click the "+ New Device" button from the menu
- Enter in the following REQUIRED fields
    - Name: anything you want (tip: keep it short)
    - Label: anything you want (tip: keep it short)
    - Device Network ID: any unique name (this will be overwritten with your device's MAC address automatically)
    - Type: "Parent_ST_Anything_Ethernet"
    - Version: "Self Published"
    - Location: your location (required!)
    - Hub: your hub (required!)

Your screen should look like the following image:

![screenshot](https://cloud.githubusercontent.com/assets/5206084/25320377/883e2d54-2874-11e7-9d7a-26774894f2c3.PNG)

- Click the Create button at the bottom of the screen
- On your phone's SmartThings app, select Things view, find and select your New Device
  - You may receive a "Device not fully configured" pop-up with the ST app.  We're about to take care of that!
- In the Arduino Device, click the "Gear Icon" in the top right of the screen
- Enter the folowing data from your Arduino
  - IP Address:  must match what you hard-coded in your Arduino sketch file
  - Port: must match what you hard-coded in your Arduino sketch file
  - MAC Address: must match your Arduino's MAC address, all uppercase, no delimiters (e.g. 06AB12CD34EF)
  - Configure the correct number of "Button Devices" to match what you defined in the Arduino Sketch.  Set to 0 if none.
Note:  If you visit the "Recently" page of your Parent Device in your ST App on your phone, you may get an annoying warning that the setup is not complete.  If you've entered all of the required data above, you can safely ignore this message.  Once it scrolls off the 'Recently' list, the pop-ups will stop.

Your screen should look like the following image:

![screenshot](https://cloud.githubusercontent.com/assets/5206084/25320424/f0a8f84c-2874-11e7-820e-98e2cd0ef9d2.PNG)

- Click "Done" at the top right
- Wait a few seconds (you can watch Live Logging in the ST IDE if you'd like) while all of the Child Devices are being automagically created, assuming your Arduino or ESP8266/ESP32 board is powered up and connected to your home network 
- Pull/Drag down on the screen to refresh the page which should now have all of your child devices created!



## SmartThings Device Handler Instructions - FOR USE WITH A THINGSHIELD
- Join your Arduino/ThingShield to your hub using your phone's SmartThings App.  It will show up as a generic "Arduino ThingShield"
- Click on My Devices from navigation menu
- Select your "Arduino ThingShield" device from the list
- Click the Edit button at the bottom of the screen
- Change the Type to "Parent_ST_Anything_ThingShield"
- Click the Update button at the bottom of the screen
- On your phone's SmartThings app, select Things view, select your Arduino Device, and pull down on the screen to refresh the page
- In the Arduino Device, click the "Gear Icon" in the top right of the screen
- Configure the correct number of "Button Devices" to match what you defined in the Arduino Sketch.  Set to 0 if none.



## Items To Be Aware Of (regardless of whether using LAN or ThingShield)
1) Please do not start changing any code before getting one of the examples up and running on both the Arduino/ESP8266/ESP32 and the Device Handlers.  It is always best to start with known working code before editing it.  This greatly reduces the amount of troubleshooting later.

2) I am moving away from the old SmartApp Multiplexer + Virtual Devices with the v2.5 release of ST_Anything. I believe the ST Composite Device Handler architecture is far superior and much simpler for all users.
  - Assuming you're keeping things fairly standard, you should never need to modify the groovy code within the Parent or Child Device Handlers!  Pretty much all changes are kept within the Arduino Sketch .ino file!
  - Child Devices are automatically created - no manual creation of Virtual Devices and no messy/complicated Multiplexer SmartApps!
    - NOTE: There appears to be a race-condition within the ST platform that sometimes results in a duplicate child device being created.  Just simply delete the extra child device and everything should be fine.  I have never seen a duplicate occur after the initial creation (i.e. after the first few minutes.)  
	UPDATE:  Apparently ST is still making backend changes to the Composite Device Handler.  As of 5/18/2017, you may see an "Error Creating Child Device" pop-up in the ST App.  This is because ST no longer allows child devices with duplicate Network IDs.  In this case, look through your Live Logging in the ST IDE to see which child device it is having an issue creating.  Delete the corresponding duplicate child using your phone's app, and then click REFRESH on the parent device.  Afterwards, the child will be created correctly.  Not sure why this happens, but it appears to be a SmartThings issue. (Note:  See 5/26/2017 update a few lines below!)
	- NOTE: This error (Error Creating Child Device) may also occur when the device handler is installed by a non-owner user of a hub. Install the device handler on the hub owner account to fix. 
  - If you delete a Parent Device, all of its children are also deleted.  PLEASE NOTE that you can simply delete any child device individually if necessary (no need to delete the Parent Device!)  If the Arduino sketch no longer sends updates for those child devices, they will not be re-created.
  ***UPDATE: As of 5/26/2017, deleting a child device will result in the parent device being broken.  The only way to fix this currently is to delete the parent device which also deletes the children.  Then you can manually create the parent again, and the children will be automagically recreated.  Unfortunately, you may need to fix your SmartApps if this issue happens to you.  I have asked SmartThings to look into fixing this issue.  Until then, try to avoid deleting child devices.***
  - You can add additional devices to the Arduino sketch at a later date.  Doing so will cause the parent to automagically create the new child devices once data from the Arduino sketch makes its way to the ST Cloud.
  - You can rename any of the Child Devices via the ST Phone App as you see fit.  Just click the gear icon within any child device in the phone app.  
  - You can assign the child devices to any "Room" you have defined to keep things organized.

3) The names of the devices you create in the Arduino setup() routine MUST MATCH EXACTLY the names the Parent Device Handler code expects.  The names are CaSe SenSiTiVe!  Do not get creative with naming in the Arduino sketch as the Child Devices will not be created.  Follow the naming convention as seen in the "ST_Anything_Multiples_xxxx.ino" sketches
  - Contact Sensors:  "contact1", "contact2", "contact3", ...
  - Alarm: "alarm1", "alarm2", "alarm3", ...
  - Motion: "motion1", "motion2", "motion3", ...
  - Smoke Detectors: "smoke1", "smoke2", "smoke3", ...
  - Switch: "switch1", "switch2", "switch3", ...
  - Dimmer Switch: "dimmerSwitch1", "dimmerSwitch2", "dimmerSwitch3", ...
  - RGB Switch: "rgbSwitch1", "rgbSwitch2", "rgbSwitch3", ...
  - RGBW Switch: "rgbwSwitch1", "rgbwSwitch2", "rgbwSwitch3", ...
  - Door Control: "doorControl1", "doorControl2", "doorControl3", ...
  - Water Sensor: "water1", "water2", "water3", ...
  - CO Detector: "carbonMonoxide1", "carbonMonoxide2", "carbonMonoxide3", ...
  - Button: "button1", "button2", "button3", ...
  - Illuminance: "illuminance1", "illuminance2", "illuminance3", ...
  - Voltage: "voltage1", "voltage2", "voltage3", ...
  - RelaySwitch: "relaySwitch1", "relaySwitch2", "relaySwitch3", ...
  - Temperature: "temperature1", "temperature2", "temperature3", ...
  - Humidity: "humidity1", "humidity2", "humidity3", ...
  - Servo: "servo1", "servo2", "servo3", ...
  - ***Special Case*** DHT22 Temp/Humid: "temphumid1" along with last two arguments being "temperature1" and "humidity1", ... 
  - ***Special Case*** DS18B20 Temperature: "temperature1" if only one sensor, "temperature" if using multiples as the 1, 2, 3 will be auto added to the end of the name

4) If you are using a W5100 Ethernet Shield with an Arduino, the MAC address must start with '06' in order to be sure you're using a ***locally administered unicast MAC address***.  Make sure that you have unique MAC addresses if you use more than one W5100 on your network.

5) When entering the MAC address into the Device Preferences in your phone's SmartThings App, ***please be sure to enter it without delimiters (i.e. NO COLONS!), and in UPPERCASE***.  It should be in the form '06AB02CD03EF' without the quotes.  The MAC address will be printed out in the Arduino IDE Serial Monitor window (115200 baud) when the board is restarted.

6) When using a NodeMCU ESP8266 board, you need to be aware of some GPIO limitations.  I have assembled my findings in this image:
![screenshot](https://cloud.githubusercontent.com/assets/5206084/26479180/53488d08-419f-11e7-824f-aa1649335c02.png)

7) When using a ESP32 board, you need to be aware of some GPIO limitations
  - GPIO 34-39 are input only and do NOT support internal pullup resistors
  - GPIO  6-11 are reserved for FLASH.  Do not use these!
  - The Arduino analogWrite() function is not supported as of 8/18/2017.  This means the EX_Switch_Dim ST_Anything Class does not support PWM output on the ESP32.

8) Button Devices - "where do they show up?"  Buttons show up in the SmartApps that use them.  The Aeon Minimote Device Handler, for example, has one tile with a remote control icon on it.  Nothing else.  It also has zero configuration options as well.  I used this as a model for adding "Button" capabilities to ST_Anything per a user request.  When you define the number of buttons (via the gear-based configuration menu in the parent DH) you are announcing to all SmartApps the number of buttons defined.  This in turn allows SmartApps like "Smart Lighting", "CoRE", and "WebCoRE" to know how many "buttons" to offer you to configure an action.  So, the easiest test is to create a new "Smart Lighting" automation, where you define the action to be based on a button "pushed" or "held" event from the Parent Device (no child devices are created for buttons).  Give it a try! I have used this successfully in the past for testing purposes.


## Final Notes for now...
Plese refer to the header files of the ST_Anything library for explanation of specific classes, constructor arguments, etc... ST_Anything devices support inverted logic, default power-on states, debounce logic, etc...  Read through the top section of the .h files found on the libraries\ST_Anything... folders for more information!

Look at the documentation in the 'ST_Anything_Multiples_xxxx.ino' files for explanation and examples of the general use of the library.  

## Older History of releases
- v1.0  2015-01-05 Initial Release
- v1.1  2015-01-11 Added st::IS_DoorControl device class & ST_Anything_Doors example code (sketch + groovy)
- v1.2  2015-02-12 Added st::EX_RCSwitch device class & ST_Anything_RCSwitch example code (sketch and groovy), included DHT and RCSwitch libraries in the Arduino\libraries\ folder of the repo to simplify initial setup for users.
- v1.3  2015-03-28 Added Furnace Alarm, Temperatures, and Relays example code (sketches and groovy code)
- v1.4  2015-04-14 Memory Optimizations
- v1.5  2015-12-06 Added Alarm_Panel MEGA 2560 example, as well as adding Smoke Detector capability
- v1.6  2017-02-11 Final release prior to the new version 2.0 baseline
- v2.0  2017-02-12 Initial release of v2.x platform with additonal support for Ethernet connectivity to SmartThings
- v2.1  2017-02-20 Added support for using the ESP-01 as WiFi communications for Arduino MEGA 2560 (SmartThings and ST_Anything libraries updated)
- v2.2  2017-03-25 Added new IS_Button class, sample sketches, updated Device Handler, etc... to support ST "Button" capability
- v2.5  2017-04-23 New SmartThings Composite Device Handler (i.e. Parent/Child Device Handlers) which eliminates the need for the Multiplexer SmartApps!  Also added Carbon Monoxide, Alarm with Strobe, and Voltage Measurement capabilities. Support for LAN devices only at this time.
- v2.6  2017-04-26 Added support for ThingShield using new Composite Device Handler.  Includes new version of SmartThings library, updates to ST_Anything library, and new ST_Anything_Multiples_Thingshield.ino sketch.  Minor tweak to EX_Alarm logic to better handle whether or not the Strobe Pin is defined.
- v2.7 2017-05-25 Added support for the Arduino W5500 Ethernet Shield. Added new ST_Anything_AlarmPanel_ESP8266WiFi.ino sketch. Revised the ST_Anything_Multiples_ESP8266WiFi.ino sketch to take into account NodeMCU ESP8266 GPIO limitations.
- v2.71 2017-05-28 Fix for Arduino + ESP01 (WiFi only) combination. Arduino IDE Serial Monitor window must now be set to 115200 baud to prevent ESP-01 timeouts. Also added support for Arduino MKR1000 board and Arduino + WiFi101 Shield (or Adafruit ATWINC1500 module).
- v2.8  2017-08-16 Added support for ESP32 microcontroller - I used the following HiLetgo ESP Board from Amazon (https://www.amazon.com/gp/product/B0718T232Z/ref=oh_aui_detailpage_o00_s00?ie=UTF8&psc=1)
- v2.9  2018-01-06 **Added Arduino OTA support for ESP8266;** added RSSI updates to the Parent Device for ESP8266, ESP32, WiFi101, and WiFiEsp; various new example sketches; support for analog RGB/RGBW LED strips; C to F and F to C temperature conversions in the Child Temperature DH; improved ESP32 WiFi stability; "Last Updated" date and time on all Child Devices; added support for the Adafruit TCS34725 Color Illuminance sensor; added noise filtering and linearization algorithm to PS_Voltage class; plus numerous small tweaks (view Github History for details)
