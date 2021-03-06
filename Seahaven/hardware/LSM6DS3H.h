#ifndef __HARDWARE_LSM6DS3H_H__
#define __HARDWARE_LSM6DS3H_H__

#include "i2c.h"
#include "gpio.h"

#define LSM6DS3H_I2C_BUS 0x00
#define GYRO_I2C_ADDRESS 0x6A

// define addresses we will be using. The rest can be left at default values
#define LSM6DS3H_FUNC_CFG_ACCESS 0x01
#define LSM6DS3H_CTRL1_XL 0x10
#define LSM6DS3H_CTRL2_G 0x11
#define LSM6DS3H_CTRL3_C 0x12
#define LSM6DS3H_CTRL6_C 0x15
#define LSM6DS3H_CTRL10_C 0x19
#define LSM6DS3H_FUNC_SRC 0x53
#define LSM6DS3H_TAP_CFG 0x58
#define LSM6DS3H_MD1_CFG 0x5E // could use MD2 if we wanted to route tilt int to INT2
#define LSM6DS3H_OUTX_L_G 0x22
#define LSM6DS3H_OUTX_H_G 0x23
#define LSM6DS3H_OUTY_L_G 0x24
#define LSM6DS3H_OUTY_H_G 0x25
#define LSM6DS3H_OUTZ_L_G 0x26
#define LSM6DS3H_OUTZ_H_G 0x27
#define LSM6DS3H_OUTX_L_XL 0x28
#define LSM6DS3H_OUTX_H_XL 0x29
#define LSM6DS3H_OUTY_L_XL 0x2A
#define LSM6DS3H_OUTY_H_XL 0x2B
#define LSM6DS3H_OUTZ_L_XL 0x2C
#define LSM6DS3H_OUTZ_H_XL 0x2D
// enable embedded functions register
#define ENABLE_EMB_FUNC_REG 0x80
// enable tilt event detection, two LSB conclude two ops (idk what they are)
#define TILT_EV_DETECT 0x23 // 0b0010 0011
// tilt calculation enable, LSB is latched interrupt
#define TILT_CALC_INT_EN 0x21 // 0b0010 0001
// Route tilt int to INT1
#define TILT_ON_INT1 0x02 // 0b0000 0010
// enable (z,y,x) on gryo and embedded funcs (tilt)
#define GYRO_ENABLE_EMB_FUNC 0x3C // 0b0011 1100
// Edge triggered, turn off high perf mode for accel
#define GYRO_EDGE_TRIG 0x90 // 0b1001 0000
// output gyro at 208Hz at full scale selection at 500 dps
#define GYRO_SPD 0x54 // 0b0101 0100

#define GYRO_GAIN 17.5; // sensitivity
#define GYRO_DT 208; //Hz

// GPIO
#define GYRO_INTERRUPT_PIN 27

// Bus startup
void LSM6DS3H_Start();
void LSM6DS3H_End();

// Gyro functions
int enableGyroTilt();     // Enables Gyro with embedded tilt function
int disableGyro();        // Turn off access to Gyro
int gyroInterruptPoll();  // Poll the gyro for an interrupt
int gyroClearInterrupt(); // Clear the interrupt on the gyro tilt
double getXRotation();
double getYRotation();
double getZRotation();
int16_t getGyroX();
int16_t getGyroY();
int16_t getGyroZ();
int16_t getAccelX();
int16_t getAccelY();
int16_t getAccelZ();
int getTiltDirection();
uint8_t readGyroReg(uint8_t addr);
double getTemp();
#endif
