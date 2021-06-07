//******************************************************************************************
//  File: PS_Adafruit_MPR121.cpp
//  Authors: Luca Masera
//
//  Summary:  PS_Adafruit_MPR121 is a class that inherits from the st::InterruptSensor class.
//
//  Create an instance of this class in your sketch's global variable section. 
//  For Example:  st::PS_Adafruit_MPR121 sensor(F("button1"), 5, 0x10, 0x20, 1000);
//
//	st::PS_Adafruit_MPR121() constructor requires the following arguments
//  - String &name - REQUIRED - the name of the object, should be "touchButtons_Kitchen", "touchButtons_Desk", etc...
//  - int8_t activatedButtonId - REQUIRED - the id of the sensor that is used to activate the touching capability
//  - uint8_t sensibility5C = 0x10, together with sensibility5D sets the sensibility of the touch sensors
//  - uint8_t sensibility5D = 0x20,
//	- long reqNumMillisHeld = 1000 - milliseconds thesholdb between "Held" and "Pushed"
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2022-09-22  Luca Masera    Original creation
//
//******************************************************************************************

#include "PS_Adafruit_MPR121.h"

#include "Constants.h"
#include "Everything.h"

namespace st {
// private

// public
// constructor
PS_Adafruit_MPR121::PS_Adafruit_MPR121(const __FlashStringHelper *name,
                                       int8_t activateButtonId,
                                       uint8_t sensibility5C,
                                       uint8_t sensibility5D,
                                       long reqNumMillisHeld)
    : InterruptSensor(name, 2, false),  // use parent class' constructor
      m_sensibility5C(sensibility5C),
      m_sensibility5D(sensibility5D),
      m_lreqNumMillisHeld(reqNumMillisHeld),
      activateButtonId(activateButtonId){}

// destructor
PS_Adafruit_MPR121::~PS_Adafruit_MPR121() {}

void PS_Adafruit_MPR121::init() {
  // get current status of motion sensor by calling parent class's
  // init() routine - no need to duplicate it here!
  InterruptSensor::init();

  Serial.println("Adafruit MPR121 Capacitive Touch sensor");

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A, m_sensibility5C, m_sensibility5D)) {
    Serial.println(F("MPR121 not found, check wiring?"));

  } else {
    Serial.println(F("MPR121 found!"));
    refresh();
  }
}

// called periodically by Everything class to ensure ST Cloud is kept up yo
// date. HOWEVER, not useful for the IS_Button.
void PS_Adafruit_MPR121::refresh() {
  if (isBlockedStatus) {
    isBlockedStatus = false;
    Serial.println("Blocked status: trying to perform a soft reset to restart the sensor.");

    this->init();
  }

  // Send 'init' message to allow Parent Driver to set numberOfButtons
  // attribute automatically
  Everything::sendSmartString(getName() + F(" init"));
}

void PS_Adafruit_MPR121::update() {  // Get the currently touched pads
// get current time 
long currentTime = millis();

// get current touch array
curr_touched = cap.touched();

// had an over current or out of range?
isBlockedStatus = cap.hadOverCurrent(curr_touched) || cap.wentOutOfRange();

if (!isBlockedStatus) {
  
  // A) if not active...
  if (!isActive) {
    // B) if the touch is to activate the sensor (ACTIVATE_BUTTON)... 
    if (bitRead(curr_touched, activateButtonId) && !(bitRead(curr_touched, activateButtonId))) {
      //    register activation time and ...
      Serial.println(F("activation touched"));
      activationStart = currentTime;
    
    } else if (!(bitRead(old_touched, activateButtonId)) && bitRead(curr_touched, activateButtonId)) {
      // C) if the activation time is bigger than the ACTIVATION_THRESOLD... 
      if (currentTime - activationStart > _ACTIVATION_THRESOLD) {
        // activate the sensor and register active time
        Serial.println(F("activated"));
        waitingActive = currentTime;
        isActive = true;
      }
    }

  // ... is active
  } else {
    // D) first check if still active, or at least hold
    if ((currentTime - waitingActive > _ACTIVE_TIMEOUT) && !isHold) {
      // if not, deactivate the sensor
      Serial.println(F("deactivated"));
      isActive = false;

    } else {
      // is active or hold, check what's going on with the buttons
      // for each button (we have 12), check the status of the bit in the touch array (ignore ACTIVATE_BUTTON)
      for (uint8_t currentButton = 0; currentButton < 12; currentButton++) {
        if (currentButton != activateButtonId) {
          // E) if positive and different from the old status then ...
          if (bitRead(curr_touched, currentButton) && !(bitRead(old_touched, currentButton))) {
            //... is a touch: register the TOUCH and store the touch time (for hold status)
            Serial.print("touched: ");
            Serial.println(currentButton);
            startTouch[currentButton] = currentTime;
            waitingActive = currentTime;
            Everything::sendSmartStringNow(getName() + String(currentButton) + _PUSHED);
            
            // F) if positive and equal to the old status then ...
            } else if (bitRead(curr_touched, currentButton) && (bitRead(old_touched, currentButton))) {
              // ... G) it could be a hold: if the HOLD_THRESOLD has been reached for the current button...
              if (!(bitRead(isHold, currentButton)) && (currentTime - startTouch[currentButton] > HOLD_THRESOLD)) {
                // ... register the HOLD and store a flag for the button
                Serial.print("held: ");
                Serial.println(currentButton);
                bitSet(isHold, currentButton);
                waitingActive = currentTime;
                Everything::sendSmartString(getName() + String(currentButton) + _HELD);
              }

            // H) if negative and different from the old status then ... 
            } else if (!(bitRead(curr_touched, currentButton)) && bitRead(old_touched, currentButton)) {
            // ... it means that there's no touch anymore 
            // I) if it was hold... 
            if (bitRead(isHold, currentButton)) {
            // ... reset the flag for the button
              Serial.print("stop held: ");
              Serial.println(currentButton);
              bitClear(isHold, currentButton);
            }
            // register the RELEASE for the button
            Serial.print("released: ");
            Serial.println(currentButton);
            Everything::sendSmartString(getName() + String(currentButton) + _RELEASED);
            }
        }
      }

      old_touched = curr_touched;
      }
    }
  }
}
  

void PS_Adafruit_MPR121::runInterrupt() {}

void PS_Adafruit_MPR121::runInterruptEnded() {}

}  // namespace st