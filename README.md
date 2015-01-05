ST_Anything
===========
Turn your Arduino into an AnyThing. ST_Anything is an Arduino library, sketch, and DeviceType that works with your SmartThings ThingShield to create an all-in-one SmartThings device.


![screenshot](https://cloud.githubusercontent.com/assets/5206084/5608219/53df3d16-944a-11e4-9791-ce42f7334990.PNG)


This package currently implements the following SmartThings Device Capabilities:
- Alarm (Siren only currently)
- Configuration
- Illuminance Measurement
- Motion Sensor
- Relative Humidity Measurement
- Switch
- Temperature Measurement
- Water Sensor
- Contact Sensor

Note: Attempting to use all of these at once on an Arduino UNO is likely to result in running out of SRAM on the UNO (the UNO only has 2 kilobytes of RAM.)  Using an Arduino MEGA 2560 with 8 kilobytes of SRAM is recommended if you want to run everything at once.


## Overview
ST_Anything consists of four parts:
- The ST_Anything.ino Arduino sketch
- The ST_Anything Arduino library
- The SmartThings (ThingShield) library - A modified, more efficient version 
- The ST_Anything.groovy DeviceType

##ST_Anything Arduino Setup Instructions
- Download the ST_Anything repository.
- This folder structure should mirror that of your local Arduino directory. Copy all the files in the repo to the corresponding local directories (detailed instructions of this process are below if needed).
  - Look inside the 'Arduino\Sketches' folder of the repo.
  - Copy and paste the 'ST_Anything' sketch folder into your local 'Arduino\Sketches' directory.
  - Look inside the 'Arduino\libraries' folder of the repo.
  - Copy and paste both the 'ST_Anything' and 'SmartThings' folders into your local 'Arduino\libraries' directory. (Note: it may be wise to rename your existing 'SmartThings' library to prevent any overwriting if you have already downloaded the official release.)
- Open the ST_Anything.ino and see if it successfully compiles.
- WARNING:  If you are using an Arduino UNO, you will most likely need to comment out some of the devices in the sketch (both in the global variable declaration section as well as the setup() function) due to the UNO's limited 2 kilobytes of SRAM.  Failing to do so will most likely result in unpredictable behavior. The Arduino MEGA 2560 has 8k of SRAM and has four Hardware Serial ports (UARTs).  If you plan on using lots of devices, get the MEGA 2560.

##ST_Anything SmartThings Device Type Installation Instructions
- Join your Arduino/ThingShield to your hub using your phone's SmartThings App.  It will show us as an "Arduino ThingShield"
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


More instructions coming soon:

For now, reference the header files of the ST_Anything library for explanation of specific classes. 

Look at the documentation in the 'ST_Anything.ino' file for explanation of general use of the library.  
