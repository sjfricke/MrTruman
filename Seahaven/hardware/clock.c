#include "clock.h"

void ClockStart() {
  I2cOpen(CLOCK_I2C_BUS);
  I2cSetSlave(CLOCK_I2C_BUS, CLOCK_ADDRESS);
  // ... any other device related setup
}

void ClockEnd() {
  // ... any device shut down calls
  I2cClose(CLOCK_I2C_BUS);
}

void ClockWriteRam(uint8_t data) {
  uint8_t s_data[4];
  s_data[0] = data;
  s_data[1] = 0x20;
  s_data[2] = 0x40;
  s_data[3] = 0x60;
  I2cWrite(CLOCK_I2C_BUS, CLOCK_RAM, s_data, 4);
}

// just prints it out
void ClockReadRam() {
  uint8_t data[4];
  data[0] = 0x00;
  data[1] = 0x00;
  data[2] = 0x00;
  data[3] = 0x00;  


  //  uint8_t* data = calloc(1, 4);
  if ( I2cRead(CLOCK_I2C_BUS, CLOCK_RAM, data, 4) == -1) {
    printf("fAILE TO READ\n");
  }

  printf("Clock Ram at: %x - %x - %x - %x\n", data[0], data[1], data[2], data[3]);
}
