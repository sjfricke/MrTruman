#include "LSM6DS3H.h"

int enableGyroTilt() {

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
    
    I2cWriteByte(LSM6DS3H_I2C_BUS, 0x10, 0x20);  
    //I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_CTRL10_C, &data);    
    // enable (z,y,x) on gryo and embedded funcs (tilt) 
    I2cWriteByte(LSM6DS3H_I2C_BUS, 0x19, 0x3C);  
    //I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_CTRL10_C, &data); 
//    if (data != GYRO_ENABLE_EMB_FUNC)
  //    printf("CTRL10_C read not correct, was 0x%x should be 0x%x", data, GYRO_ENABLE_EMB_FUNC);


    
    // Edge triggered, turn off high perf mode for accel 
    I2cWriteByte(LSM6DS3H_I2C_BUS, 0x12, 0x24);  
//    I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_CTRL6_C, &data); 


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
    
    
    // Edge triggered, turn off high perf mode for accel 
 //   I2cWriteByte(LSM6DS3H_I2C_BUS, LSM6DS3H_CTRL6_C, 0x90);  
//    I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_CTRL6_C, &data); 
//    if (data != GYRO_EDGE_TRIG)
//      printf("CTRL6_C read not correct, was 0x%x should be 0x%x", data, GYRO_EDGE_TRIG);
   

    // enable gyro at 208Hz at full scale selection at 500 dps (needed?) 
 //   I2cWriteByte(LSM6DS3H_I2C_BUS, LSM6DS3H_CTRL2_G, 0x54);

    // INT1_CTRL is kept at all zeros because we only want tilt interrupts and no others 
    // So any interrupt we get from INT1 is a tilt event. I think it will be edge triggered since  
    //  edge triggered interrupts are specified in CTRL6_c 
    
    // Not sure which register we should write to clear interrupt 

    // terminate bus
    // LSM6DS3H_End();

    return 0;
}
