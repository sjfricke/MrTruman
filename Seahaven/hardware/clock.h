// Just a demo of using the I2C with clock thing we have

#ifndef __HARDWARE_CLOCK_H__
#define __HARDWARE_CLOCK_H__

#include "i2c.h"

#define CLOCK_I2C_BUS 0x00
#define CLOCK_ADDRESS 0x68
#define CLOCK_RAM 0x09

void ClockStart();

void ClockEnd();

void ClockWriteRam(uint8_t data);

// just prints it out
void ClockReadRam();

#endif
