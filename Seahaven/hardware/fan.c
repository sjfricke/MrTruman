#include "PCA9685.h"


int fanOn(double duty){

	// TODO: Error Checking
	// Initialize Bus
	PCA9685_Start();

	// Now initialize the chip with a reset and enable the 
	// gpio enable if needed (I don't think its neccessary yet)
	uint8_t temp_reg;

	I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
	I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);

	printf("Read config register = %x _ fan_on\n", temp_reg);
	// Check if held in reset
	if((temp_reg & (PCA9685_HELD_RESET)) != 1){
		printf("Not in reset, Putting in reset and setting prescale _ fan_on\n");
		// Set into sleep mode.
		I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
		I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);
		temp_reg |= (1 << 4); // Turn off oscillator
		I2cWriteByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, temp_reg);
	} 

	// Set prescale clock to 0x1E for 200Hz PWM (5ms period)
	I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_PRESCALE_REG, 0x1E);

	// Turn on oscillator
	I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);
	temp_reg &= ~(1 << 4); 
	temp_reg &= ~(1); // Turn off all call
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, temp_reg);
	// Wait for oscillator to stabilize
	usleep(600);

	// Clear reset
	I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);
	temp_reg |= (1 << 7);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, temp_reg);
	usleep(600);


	// Read until the device is on
	// int on = 0;
	// while(on == 0){
	// 	I2cSetSlave(PCA9685_I2C_BUS, SERVO_FAN_ADDRESS);
	// 	I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);
	// 	if(temp_reg & (PCA9685_HELD_RESET) == 0){
	// 		on = 1;
	// 	}
	// }

	printf("Device is Running _ fan_on\n");

	//
	// -----
	//     |
	//      -----------------
	//    duty               1
	//

	// Bounds checking
	if((duty < 0) || (duty > 1)){
		duty = 0.0;
	}

	// Calculate the value to load into the off timer
	uint16_t off = duty * PCA9685_BIT_RESOLUTION;
	uint8_t low = off & 0xFF;
	uint8_t high = (off >> 8) & 0xFF;


	// Write with the correct duty cycle.
	I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_FAN_ON_L, 0x0);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_FAN_ON_H, 0x0);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_FAN_OFF_L, low);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_FAN_OFF_H, high);

	printf("Fan should be on now with duty cycle at %f _ fan_on\n", duty);

	// Terminate Bus
	PCA9685_End();

	return 0;
}


int fanOff(){

	// TODO: Error Checking

	// Initialize Bus
	PCA9685_Start();

	printf("Turning off fan now...\n");

	// Write all off.
	I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_FAN_ON_L, 0x0);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_FAN_ON_H, 0x0);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_FAN_OFF_L, 0x0);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_FAN_OFF_H, 0x0);

	printf("Fan should be off now.\n");

	// Terminate Bus
	PCA9685_End();

	return 0;
}



