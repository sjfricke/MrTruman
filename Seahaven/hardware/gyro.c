#include "LSM6DS3H.h"

int enableGyroTilt() {

    uint8_t data;
    // Open bus
    LSM6DS3H_Start();

    I2cSetSlave(LSM6DS3H_I2C_BUS, GYRO_ADDRESS); 
    // enable tilt event detection, two LSB conclude two ops (idk what they are) 
    //I2cWriteByte(LSM6DS3H_I2C_BUS, LSM6DS3H_FUNC_SRC, TILT_EV_DETECT);
    //I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_FUNC_SRC, &data); 
    //if (data != TILT_EV_DETECT)
      //printf("FUNC_SRC read not correct, was 0x%x should be 0x%x", data, TILT_EV_DETECT);
    
    // tilt calculation enable, LSB is latched interrupt 
    I2cWriteByte(LSM6DS3H_I2C_BUS, LSM6DS3H_TAP_CFG, TILT_CALC_INT_EN);  
    I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_TAP_CFG, &data); 
    if (data != TILT_CALC_INT_EN)
      printf("TAP_CFG read not correct, was 0x%x should be 0x%x", data, TILT_CALC_INT_EN);
    
    // Route tilt int to INT1 
    I2cWriteByte(LSM6DS3H_I2C_BUS, LSM6DS3H_MD1_CFG, TILT_ON_INT1); 
    I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_MD1_CFG, &data); 
    if (data != TILT_ON_INT1)
      printf("MD1_CFG read not correct, was 0x%x should be 0x%x", data, TILT_ON_INT1);
    
    // enable (z,y,x) on gryo and embedded funcs (tilt) 
    I2cWriteByte(LSM6DS3H_I2C_BUS, LSM6DS3H_CTRL10_C, GYRO_ENABLE_EMB_FUNC);  
    I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_CTRL10_C, &data); 
    if (data != GYRO_ENABLE_EMB_FUNC)
      printf("CTRL10_C read not correct, was 0x%x should be 0x%x", data, GYRO_ENABLE_EMB_FUNC);
    
    // Edge triggered, turn off high perf mode for accel 
    I2cWriteByte(LSM6DS3H_I2C_BUS, LSM6DS3H_CTRL6_C, GYRO_EDGE_TRIG);  
    I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_CTRL6_C, &data); 
    if (data != GYRO_EDGE_TRIG)
      printf("CTRL6_C read not correct, was 0x%x should be 0x%x", data, GYRO_EDGE_TRIG);
    // output gyro at 208Hz at full scale selection at 500 dps (needed?) 
    //I2cWriteByte(LSM6DS3H_I2C_BUS, LSM6DS3H_CTRL2_G, GYRO_SPD); 
    
    // INT1_CTRL is kept at all zeros because we only want tilt interrupts and no others 
    // So any interrupt we get from INT1 is a tilt event. I think it will be edge triggered since  
    //  edge triggered interrupts are specified in CTRL6_c 
    
    // Not sure which register we should write to clear interrupt 

    // terminate bus
    // LSM6DS3H_End();

    return 0;
}
