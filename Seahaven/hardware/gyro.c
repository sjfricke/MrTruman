#include "LSM6DS3H.h"

int disableGyro()
{
  // terminate bus
  LSM6DS3H_End();
}

/**
 * This function simply reads the gpio pin associated with the gyro tilt interrupt
 * Returns 0 for no interrupt, 1 for an interrupt
 */
int gyroInterruptPoll()
{
  // setup gpio pin for interrupt it singal
  // needs to set GPIO pin 115 (27 front facing) to input
  uint16_t interrupt_pin;
  interrupt_pin = GpioDB410cMapping(GYRO_INTERRUPT_PIN);
  return GpioGetValue(interrupt_pin);
}

/**
 * Clears the interrupt on the gyro by reading from the interrupt register
 */
int gyroClearInterrupt()
{
  // Read the FUNC_SRC Register to clear the interrupt
  uint8_t data;
  I2cSetSlave(LSM6DS3H_I2C_BUS, GYRO_I2C_ADDRESS);
  I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_FUNC_SRC, &data);
  return 0;
}

/**
 * Starts bus communication with gyro, prepares it, and readies tilt interrupt.
 */
int enableGyroTilt()
{

  uint8_t data;
  uint16_t interrupt_pin;

  // setup gpio pin for interrupt it singal
  // needs to set GPIO pin 115 (27 front facing) to input
  interrupt_pin = GpioDB410cMapping(GYRO_INTERRUPT_PIN);
  GpioEnablePin(interrupt_pin);
  GpioSetDirection(interrupt_pin, INPUT_PIN);

  // Open I2C bus
  LSM6DS3H_Start();

  I2cSetSlave(LSM6DS3H_I2C_BUS, GYRO_I2C_ADDRESS);
  I2cWriteByte(LSM6DS3H_I2C_BUS, LSM6DS3H_CTRL1_XL, 0x20);
  // enable (z,y,x) on gryo and embedded funcs (tilt)
  I2cWriteByte(LSM6DS3H_I2C_BUS, LSM6DS3H_CTRL10_C, 0x3C);
  I2cWriteByte(LSM6DS3H_I2C_BUS, LSM6DS3H_CTRL3_C, 0x04);

  // tilt calculation enable, LSB is latched interrupt
  I2cWriteByte(LSM6DS3H_I2C_BUS, LSM6DS3H_TAP_CFG, 0x21);
  I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_TAP_CFG, &data);
  if (data != TILT_CALC_INT_EN)
    printf("TAP_CFG read not correct, was 0x%x should be 0x%x", data, TILT_CALC_INT_EN);

  // Route tilt int to INT1
  I2cWriteByte(LSM6DS3H_I2C_BUS, LSM6DS3H_MD1_CFG, 0x02);
  I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_MD1_CFG, &data);
  if (data != TILT_ON_INT1)
    printf("MD1_CFG read not correct, was 0x%x should be 0x%x", data, TILT_ON_INT1);

  return 0;
}
