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
#if defined(ST_LINUX)
    typedef uint64_t time_unit;
#elif defined(ST_ARDUINO)
    typedef uint32_t time_unit;
#endif

#if defined(ST_ARDUINO)
    #define stringcpy strncpy_P
    #define stringcmp strcmp_P
    #define stringprintf sprintf_P
#else
    #define stringcpy strncpy 
    #define stringcmp strcmp
    #define stringprintf sprintf
#endif


const uint8_t MAX_DEVICES = 30;
const uint16_t UPDATES_BUFFER_SIZE = 10 * MAX_DEVICES;
const time_unit SEND_UPDATES_INTERVAL = 0; //if zero, will send every time Anything's run() method executes
const time_unit REFRESH_INTERVAL = 60000;

const uint8_t JSMN_TOKENS = 25; //number of jsmn tokens

#endif
