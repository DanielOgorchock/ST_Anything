//******************************************************************************************
//  File: PS_Adafruit_MPR121.h
//  Authors: Luca Masera
//
//  Summary:  PS_Adafruit_MPR121 is a class which implements the SmartThings
//  "Contact Sensor" device capability.
//  It inherits from the st::InterruptSensor class.
//
//  Create an instance of this class in your sketch's
//  global variable section 			  For Example:
//  st::PS_Adafruit_MPR121 sensor(F("touchButtons"), 5);
//
//	st::PS_Adafruit_MPR121() constructor requires the following arguments
//  - String &name - REQUIRED - the name of the object, should be "touchButtons_Kitchen", "touchButtons_Desk", etc...
//  - int8_t activatedButtonId - REQUIRED - the id of the sensor that is used to activate the touching capability
//  - uint8_t sensibility5C = 0x10, together with sensibility5D sets the sensibility of the touch sensors
//  - uint8_t sensibility5D = 0x20,
//	- long reqNumMillisHeld = 1000 - milliseconds thesholdb between "Held" and "Pushed"
//  - 
//(default = 1000)
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2022-09-22  Luca Masera    Original Creation
//
//******************************************************************************************

#ifndef ST_PS_ADAFRUIT_MPR121_H
#define ST_PS_ADAFRUIT_MPR121_H

#include <Adafruit_MPR121.h>

#include "InterruptSensor.h"

namespace st {
class PS_Adafruit_MPR121 : public InterruptSensor {
  private:
  unsigned long m_lTimeBtnPressed;    // time when the digital input went high
  unsigned long m_lreqNumMillisHeld;  // amount of time required to trigger "held" instead of "pushed"

  uint8_t m_sensibility5C; // couple of values used to set the default sensitivity of the sensors
  uint8_t m_sensibility5D;

  bool m_bFirstRun = true;   // used to prevent sending inadvertent button
                             // pushed/held message on startup
  bool m_bHeldSent = false;  // used to prevent sending multiple 'held' messages
  bool isBlockedStatus = false; // ued to inform if the sensor is blocked and needs a reset

  const __FlashStringHelper *_PUSHED = F(" pushed");
  const __FlashStringHelper *_HELD = F(" held");
  const __FlashStringHelper *_RELEASED = F(" released");

  u_int16_t _ACTIVATION_THRESOLD = 1000;
  u_int16_t _ACTIVE_TIMEOUT = 2000;
  u_int16_t HOLD_THRESOLD = 800;
  boolean isActive = false;


  int8_t activateButtonId;
  uint16_t curr_touched, old_touched, isTouched, isHold;
  long currentTime, activationStart, waitingActive, startTouch[12];

  Adafruit_MPR121 cap = Adafruit_MPR121();
  
  public:
  // constructor - called in your sketch's global variable declaration section
  PS_Adafruit_MPR121(const __FlashStringHelper *name, 
                    int8_t activateButtonId, 
                    uint8_t sensibility5C = 0x10, // defaults to low sensibility for almost touch
                    uint8_t sensibility5D = 0x20,
                    long reqNumMillisHeld = 1000); //defaults to 1 second  

  // destructor
  virtual ~PS_Adafruit_MPR121();

  // initialization function
  virtual void init();

  // called periodically by Everything class to ensure ST Cloud is kept up to
  // date.  Not used for IS_Button.
  virtual void refresh();

  // handles what to do when interrupt is triggered
  virtual void runInterrupt();

  // handles what to do when interrupt is ended
  virtual void runInterruptEnded();

  // override update method
  virtual void update();
  };
}  // namespace st

#endif
