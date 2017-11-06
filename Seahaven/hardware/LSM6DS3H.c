#include "LSM6DS3H.h"

void LSM6DS3H_Start() {
  I2cOpen(LSM6DS3H_I2C_BUS);
  // ... any other device related setup
}

void LSM6DS3H_End() {
  // ... any device shut down calls
  I2cClose(LSM6DS3H_I2C_BUS);
}