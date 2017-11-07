#ifndef __HARDWARE_PCA9685_H__
#define __HARDWARE_PCA9685_H__

#include "i2c.h"
#include "gpio.h"
#include <assert.h>

// Devices
#define PCA9685_I2C_BUS 0x01
#define PCA9685_SERVO_FAN_ADDRESS 0x40
#define PCA9685_RED_ADDRESS 0x50
#define PCA9685_BLUE_ADDRESS 0x44
#define PCA9685_GREEN_ADDRESS 0x48
#define PCA9685_ALL_CALL 0x70 

// Specific Registers
#define PCA9685_MODE_REG 0x00
#define PCA9685_PRESCALE_REG 0xFE
#define PCA9685_HELD_RESET (1 << 7)
#define PCA9685_BASE 0x6

#define PCA9685_FAN_ON_L 0xA
#define PCA9685_FAN_ON_H 0xB
#define PCA9685_FAN_OFF_L 0xC
#define PCA9685_FAN_OFF_H 0xD

#define PCA9685_BIT_RESOLUTION 4096

#define PCA9685_ALL_LED_ON_L 0xFA 
#define PCA9685_ALL_LED_ON_H 0xFB 
#define PCA9685_ALL_LED_OFF_L 0xFC 
#define PCA9685_ALL_LED_OFF_H 0xFD 

// set distinct all call addr for R, G, B. LSB must be 0
#define ALL_CALL_RED_ADDRESS 0xE8
#define ALL_CALL_GREEN_ADDRESS 0xE4
#define ALL_CALL_BLUE_ADDRESS 0xE2

// Below is configured for 20% duty cycle with 10% delay from start of counter 
#define ALL_LED_ON_L_VAL 0x99 
#define ALL_LED_ON_H_VAL 0x01 // 4 LSBs are LEDn_ON count 
#define ALL_LED_OFF_L_VAL 0xCC // count 
#define ALL_LED_OFF_H_VAL 0x04 
#define MODE1_VAL 0x01 // enable all_on, use internal clock

// Constants
#define TWOSECONDS 2000000

// Chip level startup
void PCA9685_Start();
void PCA9685_End();

////////////////////
// Servo commands //
////////////////////

#define SERVO_GPIO_PIN 24

#define SERVO_ON_L 0x6
#define SERVO_ON_H 0x7
#define SERVO_OFF_L 0x8
#define SERVO_OFF_H 0x9

// setups GPIO and PCA9685 for servo to be ready
// returns 0 on success
int servoStart();

// sets GPIO and PCA9685 for servo to turn off
// Assumes that servoStart() was called before at least once
// returns 0 on success
int servoStop();

// Rotates servo to extend speakers out of house
// returns 0 on success
int servoExtend();

// Rotates servo to retract speakers into the house
// returns 0 on success
int servoRetract();

//////////////////
// Fan commands //
//////////////////
int fanOn(double duty);
int fanOff();

//////////////////
// LED commands //
//////////////////
int initLED(uint8_t rgb); 
int initLEDs(); // Enables all LED drivers 
int turnOffLEDs();
int setLED(uint8_t rgb, double duty, uint16_t mask); // Set color to specific 
int turnOffLEDPWM(uint8_t rgb);


#endif
