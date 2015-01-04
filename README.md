ST_Anything
===========
Turn your Arduino into an AnyThing. ST_Anything is an Arduino library, sketch and DeviceType that works with your SmartThings ThingShield to create an all-in-one SmartThings device.

![screenshot](https://cloud.githubusercontent.com/assets/5153370/5604859/5da9edda-93a7-11e4-8e43-ce7a6d9cfa42.png)


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

##ST_Anything SmartThings Device Type Installation Instructions
- Create and account or log into the SmartThings Web IDE.
- Click on  My Device Types  from the navigation menu.
- Click on  + New SmartDevice  button.
- Fill in the  Name  field and click on the  Create  button. Don't worry about filling out everything else.
- Paste the code from the ST_Anything.groovy file.
- Click on  Save  in the IDE.
- Click on  Publish -> For Me  in the IDE.



More instructions coming soon:

For now, reference the header files of the ST_Anything library for explanation of specific classes. 

Look at the documentation in the 'ST_Anything.ino' file for explanation of general use of the library.  
