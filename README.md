History:
- v1.0 2015-01-05 Initial Release
- v1.1 2015-01-11 Added st::IS_DoorControl device class & ST_Anything_Doors example code (sketch + groovy)
- v1.2 2015-02-12 Added st::EX_RCSwitch device class & ST_Anything_RCSwitch example code (sketch and groovy), included DHT and RCSwitch libraries in the Arduino\libraries\ folder of the repo to simplify initial setup for users.
- v1.3 2015-03-28 Added Furnace Alarm, Temperatures, and Relays example code (sketches and groovy code)
- v1.4 2015-04-14 Memory Optimizations
- v1.5 2015-12-06 Added Alarm_Panel MEGA 2560 example, as well as adding Smoke Detector capability
- v1.6 2017-02-11 Final release prior to the new version 2.0 baseline
- v2.0 2017-02-12 Initial release of v2.x paltform with additonal support for Ethernet connectivity to SmartThings
- v2.1 2017-02-20 Added support for using the ESP-01 as WiFi communications for Arduino MEGA 2560 (SmartThings and ST_Anything libraries updated)

ST_Anything v2.1
================
Note: We created a ST_Anything v1.6 release on 2017-02-11 to make sure everyone can still get back to the original ThingShield-only code if necessary.  

Note: ST_Anything v2.1 was built using the Arduino IDE v1.8.1.  Please make sure to upgrade your IDE.

Turn your Arduino UNO/MEGA or NodeMCU ESP8266 into an AnyThing. ST_Anything is an Arduino library, sketch, and Device Handler that works with your Arduino/ThingShield, Arduino/W5100 Ethernet shield, Arduino/ESP-01 or a NodeMCU ESP8266 to create an all-in-one SmartThings device.

Note:  There are some significant changes as compared to the old v1.x platform.  A signiciant rewrite of the "SmartThings" Arduino library was completed to incorporate Ethernet communications support.  To use ST_Anything v2.x, you must also use all of the other supporting libaries found in this GitHub repository.  Included is a the new SmartThings v2.x Arduino library which can be used standalone (examples are included in the library), or in conjunction with the ST_Anything library.

For now, I focused on getting something together for the user community to take a look at.  All of my previous examples have not been modified yet to support the new SmartThings v2.x library.  Therefore I have removed them from this initial release to avoid confusion.  The original "ST_Anything" examples are still here to get you started. I hope to add the others back back when I have more time.  

THIS IS A WORK IN PROGRESS!  So please be patient.  The essential code is all here and has been tested.  Documentation is still lacking somewhat, so feel free to submit a pull request to improve this ReadMe as you try to get things working.



![screenshot](https://cloud.githubusercontent.com/assets/5206084/12374289/021d4434-bc65-11e5-9efa-1457365a6cd5.PNG)


This package currently implements the following SmartThings Device Capabilities:
- Alarm (Siren only currently) (digital output to a relay)
- Configuration (to set polling intervals from ST Cloud)
- Illuminance Measurement (photo resistor)
- Motion Sensor (HC-SR501 Infrared PIR)
- Relative Humidity Measurement (DHT22, DHT11)
- Switch (Sunfounder Relay - http://amzn.com/B00E0NTPP4)
- Temperature Measurement (DHT22 - requires Rob Tillaart's DHT 0.1.13 Library, included in this repo)
- Temperature Measurement (using Dallas Semiconductor DS18B20's)
- Water Sensor (http://amzn.com/B00HTSL7QC)
- Contact Sensor  (Magnetic Door Switch)
- Door Control (i.e. Garage Door Contact Sensor + Relay Output) - See 'ST_Anything_Doors' example
- RCSwitch Control (i.e. Radio Control Switch) - See 'ST_Anything_RCSwitch' example (Requires RCSwitch library, included in this repo)
- Thermocouple Temperature Measurement (via the Adafruit MAX31855 library, included in this repo)
- Smoke Detector (as a simple digital input)
- Relay Switch (implemented as a timed relay, that turns off locally - i.e. no ST cloud command necessary to turn off)

Note: Attempting to use all of these at once on an Arduino UNO R3 is likely to result in running out of SRAM on the UNO (the UNO only has 2 kilobytes of RAM.)  Using an Arduino MEGA 2560 with 8 kilobytes of SRAM is recommended if you want to run everything at once.


## Overview
ST_Anything (original example) consists of four parts:
- The ST_Anything_xxxxx.ino example sketches 
  - ST_Anything_ThingShield.ino - Arduino UNO/MEGA + ST ThingShield
  - ST_Anything_EthernetW5100.ino - Arduino UNO/MEGA + W5100 Ethernet Shield
  - ST_Anything_WiFiEsp.ino - Arduino MEGA + ESP-01 WiFi module with "AT Firmware"
  - ST_Anything_ESP8266WiFi.ino - NodeMCU v1.0 ESP8266-12e development board (no Arduino!)
  - ST_Anything_ESP01WiFi.ino - ESP-01 (ESP8266-01) module (no Arduino!)
- The ST_Anything Arduino libraries
- The SmartThings library - A modified, more efficient version, now with added support for LAN-to-Hub based communications too! 
- The SmartThings Device Handlers that correspond to the sketches above.
  - ST_Anything_ThingShield.device.groovy (ThingShield)
  - ST_Anything_Ethernet.device.groovy (Arduino/W5100, Arduino/ESP-01, NodeMCU ESP8266-12e, ESP-01) 

##ST_Anything Arduino Setup Instructions - FOR USE WITH A THINGSHIELD
- Download the ST_Anything repository.
- This folder structure should mirror that of your local Arduino directory. 
  - On Mac, it's located in `~/Documents/Arduino/`.
  - On Windows, it's located in `C:\My Documents\Arduino`.
- Look inside the `Arduino/Sketches` folder of the repo.
- Copy and paste all of the `ST_Anything...` sketch folders into your local Arduino sketches directory. If you haven't created any sketches, you may not see the folder. In this case, feel free to create it.
- Look inside the `Arduino/libraries` folder of the repo.
- Copy and paste both the `ST_Anything` and `SmartThings` (as well as all of the other library folders) into your local Arduino libraries directory. 
- Open the ST_Anything_ThingsShield.ino and see if it successfully compiles.
- WARNING:  If you are using an Arduino UNO, you will most likely need to comment out some of the devices in the sketch (both in the global variable declaration section as well as the setup() function) due to the UNO's limited 2 kilobytes of SRAM.  Failing to do so will most likely result in unpredictable behavior. The Arduino MEGA 2560 has 8k of SRAM and has four Hardware Serial ports (UARTs).  If you plan on using lots of devices, get the MEGA 2560.

##ST_Anything SmartThings Device Handler Installation Instructions - FOR USE WITH A THINGSHIELD
- Join your Arduino/ThingShield to your hub using your phone's SmartThings App.  It will show up as a generic "Arduino ThingShield"
- Create an account and/or log into the SmartThings Developers Web IDE.
- Click on "My Device Handlers" from the navigation menu.
- Click on  "+ New Device Handler" button.
- Select the "From Code" Tab near the top of the page
- Paste the code from the ST_Anything_ThingShield.device.groovy file from this repo (.\ST_Anything\Groovy\ST_Anything\DeviceHandlers\)
- If you commented out any of the devices in the sketch, be sure to comment out the corresponding tiles & preferences in the ST_Anything.groovy code as well.
- Click on "Create" near the bottom of the page.
- Click on  "Save"  in the IDE.
- Click on  "Publish" -> "For Me"  in the IDE.
- Click on My Devices from navigation menu
- Select your "Arduino ThingShield" device from the list
- Click the Edit button at the bottom of the screen
- Change the Type to "ST_Anything_ThingShield"
- Click the Update button at the bottom of the screen
- On your phone, log out of SmartThings in the app, and then log back into SmartThings to refresh its settings
- Your Arduino Device Tile should now look like the image above in this ReadMe
- Be sure to go into the Preferences section to set the polling rates for the sensors.  These are sent to the Arduino if you press the Configure tile.  (Note:  Currently, these settings do not persist after an Arduino reboot.)


##Ethernet (Arduino/W5100, Arduino/ESP-01, and ESP8266) Examples
The steps for using the Arduino/W5100, Arduino/ESP-01, and NodeMCU ESP8266 sample code is very similar to above, with the added steps of staic IP assignements, MAC addresses, SSID and Passwords, etc...
For now, please refer to the SmartThings library's Readme.md for these details https://github.com/DanielOgorchock/ST_Anything/tree/master/Arduino/libraries/SmartThings 


##Updated SmartThings ThingShield Library
As mentioned previously, the "SmartThings" v2.x library was extensively modified for Ethernet support.  Please see the readme.md file for that particular library for more detailed information. 

##How do I create and expose more than 1 of a single capability?
By default, SmartThings only allows each device to have one of each capability.  That means you can't create a device with 6 "Contact Sensor" capabilities and have normal SmartApps be able to use each of the 6 sensors for normal processing.  There is a work-around for this which I call a Multiplexer SmartApp.  When used in conjuction with a virtual device for each of the 6 Arduino "Contact Sensor" devices, we can finally allow normal SmartApps to interact with each sensor, switch, etc..

As an example, here are the basic steps to use "ST_Anything_Doors_ThingShield"

1) Load ST_Anything_Dooors_ThingShield.ino on your Arduino (requires all of the associated library files to have also been installed in your Arduino Libraries folder.)

2) Create a new "ST_Anything_Doors_ThingShield" Device Handler in the ST IDE (under Device Handlers) and paste in the ST_Anything_Doors_ThingShield.device.groovy code. Save and Publish. 

3) In the ST IDE (under Devices), change the Device Handler of your Arduino to your new "ST_Anything_Doors_ThingShield" DeviceType from step 2. 

4) Create a new "Virtual Contact Sensor" Device Handler in the ST IDE (under Device Handlers) and paste in the VirtualContactSensor.device.groovy code. Save and Publish.

5) In the ST IDE (under Devices), create a new device (call it whatever you want) and assign it to use the "Virtual Contact Sensor" Device Handler from step 4.

6) Repeat step 5 until you have created a virtual "contact sensor" device for each of the Arduino's real contact sensor inputs.

7) In the ST IDE (under Smart Apps), create a new Smart App called "ST_Anything_Doors_Multiplexer" and paste in the groovy code from ST_Anything_Doors_Multiplexer.smartapp.groovy. Save and Publish.

8) Using your phone/tablet, start the SmartThings app and "Add a new SmartApp" - select the "ST_Anything_Doors_Multiplexer" app that you created in step 7.

9) Configure the new smart app to use the Virtual Contact Sensor devices you created in steps 5/6. Configure the smart app to use the Arduino device you revised in step 3. Save the settings.

I have skipped the virtual garage "Door Control" devices, but you can repeat steps 4/5/6 to create the two virtual garage door devices if desired using the corresponding groovy code. Then you can add them to the SmartApp as well. These virtual devices are a little more complicated as they are two-way devices.

Basically, what is happening here is the following:

-Arduino reads real world data and sends to ST

-Events are raised in the ST_Anything_Dooors_ThingShield device

-The ST_Anything_Dooors_Multiplexer smart app sees these events, and in turn, updates the virtual contact sensor devices with the current state of the Arduino's inputs.

-Any other smart apps that are looking at the virtual contact sensor devices will then receive an update and act accordingly.


##Final Notes for now...
Plese refer to the header files of the ST_Anything library for explanation of specific classes, constructor arguments, etc... 

Look at the documentation in the 'ST_Anything_ThingShield.ino' file for explanation of general use of the library.  
