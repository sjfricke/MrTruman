#include "LSM6DS3H.h"

int disableGyro(){
  // terminate bus
  LSM6DS3H_End();
  return 0;
}

// Returns 1 if it thinks the tilt direction is right,
// -1 if left, 0 if cant determine
int getTiltDirection(){
  int x = 0;
  int z = 0;
  for (int i = 0; i < 4; i++){
    x += getAccelX();
    z += getAccelZ();
  }
  x = x / 4;
  z = z / 4;
  int16_t x_avg = (int16_t)x;
  int16_t z_avg = (int16_t)z;
  // Arrived at these values from trial and error (MACHINE LEARNING)
  if (x_avg > 1000 && z_avg < 10000 && z_avg > 0){
    return -1;
  }
  else if (x_avg < -1000 && z_avg < 10000 && z_avg > 0){
    return 1;
  }
  return 0;
}

// Read the temperature from the gyroscope
double getTemp(){
  int16_t tempcode = (readGyroReg(0x20)) + (readGyroReg(0x21) << 8);
  return (tempcode/16.0)+25;
}

uint8_t readGyroReg(uint8_t addr){
  uint8_t data;
  I2cSetSlave(LSM6DS3H_I2C_BUS, GYRO_I2C_ADDRESS);
  I2cReadByte(LSM6DS3H_I2C_BUS, addr, &data);
  return data;
}

int16_t getGyroX(){
  uint8_t dataL, dataH;
  dataL = readGyroReg(LSM6DS3H_OUTX_L_G);
  dataH = readGyroReg(LSM6DS3H_OUTX_H_G);
  // concatenate
  return (dataL | (dataH << 8));
}

int16_t getGyroY(){
  uint8_t dataL, dataH;
  dataL = readGyroReg(LSM6DS3H_OUTY_L_G);
  dataH = readGyroReg(LSM6DS3H_OUTY_H_G);
  // concatenate
  return (dataL | (dataH << 8));
}

int16_t getGyroZ(){
  uint8_t dataL, dataH;
  dataL = readGyroReg(LSM6DS3H_OUTZ_L_G);
  dataH = readGyroReg(LSM6DS3H_OUTZ_H_G);
  // concatenate
  return (dataL | (dataH << 8));
}

int16_t getAccelX(){
  uint8_t dataL, dataH;
  dataL = readGyroReg(LSM6DS3H_OUTX_L_XL);
  dataH = readGyroReg(LSM6DS3H_OUTX_H_XL);
  // concatenate
  return (dataL | (dataH << 8));
}

int16_t getAccelY(){
  uint8_t dataL, dataH;
  dataL = readGyroReg(LSM6DS3H_OUTY_L_XL);
  dataH = readGyroReg(LSM6DS3H_OUTY_H_XL);
  // concatenate
  return (dataL | (dataH << 8));
}

int16_t getAccelZ(){
  uint8_t dataL, dataH;
  dataL = readGyroReg(LSM6DS3H_OUTZ_L_XL);
  dataH = readGyroReg(LSM6DS3H_OUTZ_H_XL);
  // concatenate
  return (dataL | (dataH << 8));
}

/**
 * This function simply reads the gpio pin associated with the gyro tilt interrupt
 * Returns 0 for no interrupt, 1 for an interrupt
 */
int gyroInterruptPoll(){
  // setup gpio pin for interrupt it singal
  // needs to set GPIO pin 115 (27 front facing) to input
  uint16_t interrupt_pin;
  interrupt_pin = GpioDB410cMapping(GYRO_INTERRUPT_PIN);
  return GpioGetValue(interrupt_pin);
}

/**
 * Clears the interrupt on the gyro by reading from the interrupt register
 */
int gyroClearInterrupt(){
  // Read the FUNC_SRC Register to clear the interrupt
  uint8_t data;
  I2cSetSlave(LSM6DS3H_I2C_BUS, GYRO_I2C_ADDRESS);
  I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_FUNC_SRC, &data);
  return 0;
}

/**
 * Starts bus communication with gyro, prepares it, and readies tilt interrupt.
 */
int enableGyroTilt(){

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
  I2cWriteByte(LSM6DS3H_I2C_BUS, LSM6DS3H_CTRL3_C, 0x44); // SET BDU and 3-WIRE selection.

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

  // turn on gyro
  I2cWriteByte(LSM6DS3H_I2C_BUS, LSM6DS3H_CTRL2_G, 0x54);
  return 0;
}
