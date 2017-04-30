#ifndef ST_CONSTANTS_H
#define ST_CONSTANTS_H

#include <stdint.h>

#define ST_LINUX
//#define ST_ARDUINO

#ifndef ST_ARDUINO
    #define F(x) (x)
#endif

#define ST_DEBUG

typedef uint8_t pin_unit;

const uint8_t MAX_DEVICES = 30
const uint16_t UPDATES_BUFFER_SIZE = 10 * MAX_DEVICES 
const time_unit SEND_UPDATES_INTERVAL = 0 //if zero, will send every time Anything's run() method executes
const time_unit REFRESH_INTERVAL = 60000;

#endif
