#include "PCA9685.h"


const uint8_t pinmaps[10] = {5,4,3,2,1,14,13,12,11,10};




/*static void writeLOG(char* msg) {
    FILE *LOG;
    LOG = fopen("LED_LOG", "a");
    fprintf(LOG, msg);
    fprintf(LOG, "\n");
    fclose(LOG);
}*/

/*static void check(int val) {
    if (val == 0) {
        writeLOG("SOME MSG");
    }
}*/

int turnOffLEDPWM(uint8_t rgb) {
    I2cSetSlave(PCA9685_I2C_BUS, rgb);
    I2cWriteByte(PCA9685_I2C_BUS, PCA9685_ALL_LED_OFF_H, 0x10);    
    return 0;
}


int setLED(uint8_t rgb, double duty, uint16_t mask){

    // Bounds checking
    if((duty < 0) || (duty > 1)){
        duty = 0.0;
    }

    // Calculate the value to load into the off timer
    uint16_t off = duty * PCA9685_BIT_RESOLUTION;
    uint8_t low = off & 0xFF;
    uint8_t high = (off >> 8) & 0xFF;


    if(mask == 0x3FF){
       I2cSetSlave(PCA9685_I2C_BUS, rgb);
       I2cWriteByte(PCA9685_I2C_BUS, PCA9685_ALL_LED_ON_L, 0x0);    
       I2cWriteByte(PCA9685_I2C_BUS, PCA9685_ALL_LED_ON_H, 0x0);    
       I2cWriteByte(PCA9685_I2C_BUS, PCA9685_ALL_LED_OFF_L, low);    
       I2cWriteByte(PCA9685_I2C_BUS, PCA9685_ALL_LED_OFF_H, high);    
       return 0;
    }


    int j;
    // Loop through and set the ones that need setting 
    for(j = 0; j < 10; j++){
        // Write PWM values
        if(mask & (1 << j)){
            uint8_t offset = pinmaps[j];
            I2cSetSlave(PCA9685_I2C_BUS, rgb);
            I2cWriteByte(PCA9685_I2C_BUS, PCA9685_BASE + (4*offset), 0x0);    
            I2cWriteByte(PCA9685_I2C_BUS, PCA9685_BASE + (4*offset+1), 0x0);  
            I2cWriteByte(PCA9685_I2C_BUS, PCA9685_BASE + (4*offset+2), low);
            I2cWriteByte(PCA9685_I2C_BUS, PCA9685_BASE + (4*offset+3), high);
        } 

    }



    return 0;
}

int initLED(uint8_t rgb) {

    uint8_t temp_reg;

    // Get all call address and error checking
    // if (LED_addr == PCA9685_RED_ADDRESS) {
    //     LED_all_call_addr = ALL_CALL_RED_ADDRESS;
    // } else if (LED_addr == PCA9685_GREEN_ADDRESS) {
    //     LED_all_call_addr = ALL_CALL_GREEN_ADDRESS;
    // } else if (LED_addr == PCA9685_BLUE_ADDRESS) {
    //     LED_all_call_addr = ALL_CALL_BLUE_ADDRESS;
    // } else {
    //     return -1;
    // }

    PCA9685_Start();

    // setting up led driver with PWM 20% and ALL_ON
    I2cSetSlave(PCA9685_I2C_BUS, rgb); 


    // Turn on oscillator
    I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);
    temp_reg &= ~(1 << 4); 
    I2cWriteByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, temp_reg);
    // Wait for oscillator to stabilize for at max 500uS, im just paranoid (mek)
    usleep(600);

    // Clear reset
    I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);
    temp_reg |= (1 << 7);
    I2cWriteByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, temp_reg);
    usleep(600);

    //I2cWriteByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, MODE1_VAL);
    //I2cWriteByte(PCA9685_I2C_BUS, PCA9685_ALL_CALL, LED_all_call_addr);

    // Terminate Bus
//    PCA9685_End();
    
    return 0;
}
 

int turnOffLEDs() {
    turnOffLEDPWM(PCA9685_RED_ADDRESS);
    turnOffLEDPWM(PCA9685_GREEN_ADDRESS);
    turnOffLEDPWM(PCA9685_BLUE_ADDRESS);
    return 0;
}

int initLEDs() {
    uint8_t red, blue, green;
    red = initLED(PCA9685_RED_ADDRESS);
    blue = initLED(PCA9685_BLUE_ADDRESS);
    green = initLED(PCA9685_GREEN_ADDRESS);
    if (red < 0 || green < 0 || blue < 0) {
        return -1;
    }
    return 0;
}
