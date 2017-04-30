#ifndef ST_CONSTANTS_H
#define ST_CONSTANTS_H

#include <stdint.h>

#define ST_LINUX
//#define ST_ARDUINO

#ifndef ST_ARDUINO
    #define F(x) (x)
#endif

typedef uint8_t pin_unit;

#endif
