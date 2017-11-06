#include "PCA9685.h"

void PCA9685_Start() {
  I2cOpen(PCA9685_I2C_BUS);
  //I2cSetSlave(PCA9685_I2C_BUS, PCA9685_ADDRESS);
  // ... any other device related setup
}

void PCA9685_End() {
  // ... any device shut down calls
  I2cClose(PCA9685_I2C_BUS);
}
