/*
  This is a general purpose file for using the GPIO...generally
 */
#ifndef __HARDWARE_GPIO_H__
#define __HARDWARE_GPIO_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HIGH "1"
#define LOW "0"
#define OUT "out"
#define IN "in"

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_UNEXPORT_PATH "/sys/class/gpio/unexport"

// The Dragonboard 410c has its own mapping of
// GPIO pins on the J8 40-pin header to the pin
// recognized in Linux. This function is used
// to get the Linux pin with the J8 pin
// Returns NULL of invalid pin
char* GpioDB410cMapping(int header_pin);

// Enables access to GPIO pin
// Return 0 if successful, otherwise failed
int GpioEnablePin(char* pin);

// Disables access to GPIO pin
// Return 0 if successful, otherwise failed
int GpioDisablePin(char* pin);

// Sets the direction of pin passed in
// Input direction  == 0
// Output direction == 1
// Return 0 if successful, otherwise failed
int GpioSetDirection(char* pin, int direction);

// Gets the direction of pin passed in
// Invalid pin      == -1
// Input direction  ==  0
// Output direction ==  1
int GpioGetDirection(char* pin);

// TODO: skip validation to increase speed?
// Sets the value of pin passed in
// Return 0 if successful, otherwise failed
int GpioSetValue(char* pin, int value);

// Gets the value of pin passed in
// Returns -1 for invalid pin
int GpioGetValue(char* pin);

#endif
