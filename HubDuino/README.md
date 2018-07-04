HubDuino v1.1.2
================

**WARNING** - If using an ESP8266, make sure you are using v2.3 of the Arduino ESP8266 Board manager (v2.4/2.4.1 introduced a memory leak causing ST_Anythign to crash after an hour or two.) 

**Recent History:**
2018-07-04 HubDuino v1.1.2 - Added support for I2C devices - AM2320(temp, humid), TSL2561(lux), MAX44009(lux), BH1750(lux), BME280(temp,humid,pressure), BMP280(temp, pressure), and improved TCS34725(color lux). Added ST_Anything_I2C_ESP8266WiFi.ino example sketch to demonstrate I2C sensors.
2018-06-24 HubDuino v1.1.1 - Added Servo Motor support
2018-06-05 HubDuino v1.1 - Converted to use Hubitat Composite Device Driver Model

## Architecture Flow Chart

![screenshot](https://user-images.githubusercontent.com/5206084/41016005-e382479e-691a-11e8-871f-e62964c0e0c0.png)

Note: The HubDuino v1.1.1 release is based on the ST_Anything v2.9.1 baseline and was built using the Arduino IDE v1.8.5.  Please make sure to upgrade your IDE.

Turn your Arduino UNO/MEGA/MKR1000, NodeMCU ESP8266, or ESP32 into Anything you can imagine! HubDuino/ST_Anything is an Arduino library, sketch, and Device Handlers that works with your hardware to create an all-in-one SmartThings device. 
- Arduino with SmartThings ThingShield
- Arduino with W5100 Ethernet shield
- Arduino with W5500 Ethernet shield
- Arduino with WiFi101 Shield (or Adafruit ATWINC1500 module)
- Arduino MKR1000 (has integrated WiFi101)
- Arduino with ESP-01 for WiFi
- Standalone NodeMCU v1.0 ESP8266-12e
- Standalone ESP-01 (or really any ESP8266 based board)
- Standalone ESP32 board

THIS DOCUMENT IS A WORK IN PROGRESS!  The essential code is all here and has been tested.  Documentation is still lacking somewhat, so feel free to submit a pull request to improve this ReadMe as you try to get things working.


This package currently implements the following Hubitat Device Capabilities:
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
HubDuino/ST_Anything consists of four main parts:
- The HubDuino/ST_Anything example sketches 
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
- The HubDuino Parent + Child Drivers that support sketches above.
  - hubduino-parent-ethernet.groovy (LAN-to-Hub, - automagically creates child devices based on your Arduino sketch,Arduino/W5100/W5500, Arduino/ESP-01, NodeMCU ESP8266-12e, ESP-01, ESP32, MKR1000)
  - hubduino-parent-thingshield.groovy (Thingshield-to-Hub - automagically creates child devices based on your Arduino sketch)
  - child-xxxxxx.groovy (get these from the ST_Anything SmartThings folder at https://github.com/DanielOgorchock/ST_Anything/tree/master/devicetypes/ogiewon)
    - currently 18 child device handlers are available!

## Pre-Requisites for using LAN-to-HUB Ethernet connectivity (Arduino/W5100, Arduino/ESP-01, NodeMCU ESP8266, ESP-01, ESP32)
- Your Hubitat HUB must have a Static TCP/IP Address assigned via your router's DHCP Server. Since this procedure varies by router model, Please Google it!
- ***The Hubitat hub listens on port 39501.  This is different than the ST hub which uses 39501 - MODIFY YOUR ARDUINO SKETCH ACCORDINGLY!***
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
    - You must ensure your Hubitat hub's LAN IP address does not change.  Use your router's static DHCP assignment feature to make sure your ST hub always gets the same IP Address!  Enter that address in the corresponding line in the sketch.
    - ***The Hubitat hub listens on port 39501.  Make sure your Arduino sketch reflects this port number change!***
    - The Arduino must be assigned a static TCP/IP address, Gateway, DNS, Subnet Mask, MAC Address(W5100/W5500 only), SSID+Password(ESP8266,ESP01,ESP32,WiFi101,MKR1000)
    - *** NOTE: If using the W5100 Shield, YOU MUST ASSIGN IT A UNIQUE MAC ADDRESS in the sketch! Please leave the first octet in the MAC Address '06' as certain MAC addresses are UNICAST while others are MULTICAST. Your MAC must be UNICAST and be a 'Locally Administered Address' Please see https://en.wikipedia.org/wiki/MAC_address#Address_details for more information ***
    - *** NOTE: If using the W5500 Shield, YOU MUST ASSIGN IT A UNIQUE MAC ADDRESS in the sketch! Use the one packaged with the W5500 shield.
    - Note: If using an ESP-01 for WiFi-connecticity (i.e. as a WiFi dongle/shield) with an Arduino, the example assumes you're using an Arduino MEGA 2560. Attach the ESP-01 to Hardware Serial "Serial1" and make sure your ESP01 has current 'AT Firmware' installed.
  - Your IDE Serial Monitor Window must be set to 115200 baud  **** Please note recent change to 115200 ****
  - With the Serial Monitor window open, load your sketch and watch the output
  - The MAC Address will be printed out in the serial monitor window. Write this down as you will need it to configure the Device using your web browser connected to the Hubitat hub. (Note: MAC Address must later be entered with no delimeters (i.e. NO COLONS!) in the form of "06AB23CD45EF" (all UPPERCASE and without quotes!))

WARNING:  If you are using an Arduino UNO, you may need to comment out some of the devices in the sketch (in the setup() function) due to the UNO's limited 2 kilobytes of SRAM.  Failing to do so will most likely result in unpredictable behavior. The Arduino MEGA 2560 has 8k of SRAM and has four Hardware Serial ports (UARTs).  If you plan on using many devices, get an Arduino MEGA 2560, a MKR1000, a NodeMCU v1.0 ESP8266-12e, or a ESP32 board.

## Hubitat Elevation Hub Setup Instructions
- Using your Web Browser (Chrome works well) connect to your Hubitat Hub
- Click on "Drivers Code" from the left hand navigation menu
- Click on "+New Driver" button in top right corner
- One at a time, Copy, Paste, and Save the source code for ALL of the "child-*.groovy" and "hubduino-parent-*.groovy" Drivers found in https://github.com/DanielOgorchock/ST_Anything/tree/master/HubDuino/Drivers
- You should now have all of the necessary Hubitat Drivers added to your Hubitat Elevation Hub

Your "Drivers Code" page should now look similar to the following
![screenshot](https://user-images.githubusercontent.com/5206084/41035111-a3dbac5e-6959-11e8-963c-60c33a44986c.png)


## HubDuino Parent Ethernet Device Instructions - FOR USE WITH LAN-to-HUB WiFi/Cat5 Ethernet Devices
- Click on "Devices" from the left hand navigation menu
- Click the "+ Add Virtual Device" button in top right corner
- 
- Enter in the following REQUIRED fields
    - Device Name: anything you want (tip: keep it short as this will be the prefix for all Child Devices that are created)
    - Device Label: anything you want (tip: keep it short)
    - Device Network ID: any unique name (this will be overwritten with your device's MAC address automatically)
    - Type: "HubDuino Parent Ethernet"

Your screen should look like the following image just before you click Save:
![screenshot](https://user-images.githubusercontent.com/5206084/41034615-21f17256-6958-11e8-84cf-1f22eb7969eb.png)

Afterwards, go into the device page of the Parent and enter in the highlighted information for your ST_Anything device

![screenshot](https://user-images.githubusercontent.com/5206084/41034778-aeb2160a-6958-11e8-818e-0ca25a2c9f98.png)

- Click the SAVE button at the bottom of the screen.  This will cause a REFRESH command to be sent to your ST_Anything microcontroller, which in turn will send a status update for every device you configured in your sketch's setup() routine.  These updates from ST_Anything will cause child devices to be created if they do not yet exist.
	
## Verify your Child Devices are automagically created	
- Click on "Devices" from the left hand navigation menu
- You should see your "HubDuino Parenr Ethernet" in the list
- For every device you created in your sketch, you see see a corresponding Child Device under the Parent
- Select the "Logs" from the left hand navigation menu to view debug data from the Parent Device and Child Devices




## HubDuino Parent ThingShield Device Instructions - FOR USE WITH A THINGSHIELD
- Join your Arduino/ThingShield to your hub using the hub's "Discover Devices" feature.  It will show up as a generic "Device" or a "SmartThings sensor"
- Click on Devices from navigation menu
- Select your "Arduino ThingShield" device from the list
- Change the Type to "HubDuino Parent ThingShield"
- Configure the correct number of "Button Devices" to match what you defined in the Arduino Sketch.  Set to 0 if none.
- Click the SAVE button at the bottom of the screen.  This will cause a REFRESH command to be sent to your ST_Anything microcontroller, which in turn will send a status update for every device you configured in your sketch's setup() routine.  These updates from ST_Anything will cause child devices to be created if they do not yet exist.




## Items to be aware of
1) Please do not start changing any code before getting one of the examples up and running on both the Arduino/ESP8266/ESP32 and the Device Handlers.  It is always best to start with known working code before editing it.  This greatly reduces the amount of troubleshooting later.

2) Hubitat now supports Composite (i.e. Parent/Child) Device Drivers.  The Parent Device takes care of creating, updating, and deleting child devices
  - Assuming you're keeping things fairly standard, you should never need to modify the groovy code within the Parent / Child Drivers!  Pretty much all changes are kept within the Arduino Sketch .ino file!
  - Child Devices are automatically created by the HubDuino Parent Device - no manual creation of Virtual Devices
  - If you delete the HubDuino Parent Device, all of its children are also deleted.  PLEASE NOTE that you can simply delete any child device individually if necessary (no need to delete the Parent Device!)  If the Arduino sketch no longer sends updates for those child devices, they will not be re-created.
  - You can add additional devices to the Arduino sketch at a later date.  Doing so will cause the Parent Device to automagically create the new child devices once data from the Arduino sketch makes its way to the Hubitat hub.
  - You can rename any of the Child Devices via the Hubitat web IDE as you see fit.  Just select a Child Device, and simply change its "Device Label" field and click save.  
  - Do NOT change the Child Devices' Device Network ID - The Service Manager uses this field to link the Child device to the Parent device

3) The names of the devices you create in the Arduino setup() routine MUST MATCH EXACTLY the names the HubDuino Parent Driver code expects.  The names are CaSe SenSiTiVe!  Do not get creative with naming in the Arduino sketch as the Child Devices will not be created.  Follow the naming convention as seen in the "ST_Anything_Multiples_xxxx.ino" sketches
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

5) When entering the MAC address into the Device Preferences in the Hubitat hub parent device web page, ***please be sure to enter it without delimiters (i.e. NO COLONS!), and in UPPERCASE***.  It should be in the form '06AB02CD03EF' without the quotes.  The MAC address will be printed out in the Arduino IDE Serial Monitor window (115200 baud) when the board is restarted.

6) When using a NodeMCU ESP8266 board, you need to be aware of some GPIO limitations.  I have assembled my findings in this image:
![screenshot](https://cloud.githubusercontent.com/assets/5206084/26479180/53488d08-419f-11e7-824f-aa1649335c02.png)

7) When using a ESP32 board, you need to be aware of some GPIO limitations
  - GPIO 34-39 are input only and do NOT support internal pullup resistors
  - GPIO  6-11 are reserved for FLASH.  Do not use these!
  - The Arduino analogWrite() function is not supported as of 8/18/2017.  This means the EX_Switch_Dim ST_Anything Class does not support PWM output on the ESP32.

8) Button Devices - "where do they show up?"  Buttons show up in the Apps that use them. When you define the number of buttons (via the HubDuino Parent Device option) you are announcing to all Apps the number of buttons defined.  This in turn allows Apps like "Simple Lighting" and "Rule Machine" to know how many "buttons" to offer you to configure an action.  So, the easiest test is to create a new "Simple Lighting" automation, where you define the action to be based on a button "pushed" or "held" event from the HubDuino Parent Device (no child devices are created for buttons).  Give it a try! I have used this successfully in the past for testing purposes.


## Final Notes for now...
Plese refer to the header files of the ST_Anything library for explanation of specific classes, constructor arguments, etc... ST_Anything devices support inverted logic, default power-on states, debounce logic, etc...  Read through the top section of the .h files found on the libraries\ST_Anything... folders for more information!

Look at the documentation in the 'ST_Anything_Multiples_xxxx.ino' files for explanation and examples of the general use of the library.  
