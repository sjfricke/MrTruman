#include "PCA9685.h"


int servoOff(){

	// TODO: Error Checking

	// Initialize Bus
	PCA9685_Start();

	printf("Turning off servo now...\n");

	// Write all off.
	I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_ON_L, 0x0);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_ON_H, 0x0);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_OFF_L, 0x0);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_OFF_H, 0x0);

	printf("Servo should be off now.\n");

	// Terminate Bus
	PCA9685_End();

	return 0;
}

int extendServo(){

	// TODO: Error Checking

	// Initialize Bus
	PCA9685_Start();

	// Now initialize the chip with a reset and enable the 
	// gpio enable if needed (I don't think its neccessary yet)
	uint8_t temp_reg;

	I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
	I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);

	printf("Read config register = %x _ extend_servo\n", temp_reg);
	// Check if held in reset
	if((temp_reg & (PCA9685_HELD_RESET)) != 1){
		printf("Not in reset, Putting in reset and setting prescale _ extend_servo\n");
		// Set into sleep mode.
		I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
		I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);
		temp_reg |= (1 << 4); // Turn off oscillator
		I2cWriteByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, temp_reg);
	} 

	// Set prescale clock to 0x81 for 47Hz PWM (21.3ms period)
	I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_PRESCALE_REG, 0x81);

	// Turn on oscillator
	I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);
	temp_reg &= ~(1 << 4); 
	temp_reg &= ~(1); // Turn off oscillator
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

	printf("Device is Running _ extend_servo\n");

	// Need to turn on the outputs for a 1.3ms on time for the 21.3ms period.
	// On Value = 0x0 (0,0)
	// Off Value = 0xFA *(0,FA)
	//
	// -----
	//     |
	//      -----------------
	//    1.3               21.3
	//
	I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_ON_L, 0x0);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_ON_H, 0x0);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_OFF_L, 0xFA);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_OFF_H, 0x0);


	printf("Extending servo 1.3ms - sleeping _ extend_servo\n");
	// Wait two seconds (or however long the it takes to extend)
	//usleep(TWOSECONDS);

	printf("Waking up, shutting down servo _ extend_servo\n");

	// Turn off the outputs.
	// Can turn into a single write once verified.
//	I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
//	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_ON_L, 0x0);
//	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_ON_H, 0x0);
//	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_OFF_L, 0x0);
//	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_OFF_H, 0x0);

	// Terminate Bus
	PCA9685_End();

	return 0;
}

int retractServo(){


	PCA9685_Start();

	// Now initialize the chip with a reset and enable the 
	// gpio enable if needed (I don't think its neccessary yet)
	printf("Set slave address _ retract_servo\n");
	uint8_t temp_reg;

	I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
	I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);

	printf("Read config register = %x _ retract_servo\n", temp_reg);
	// Check if held in reset
	if((temp_reg & (PCA9685_HELD_RESET)) != 1){
		printf("Not in reset, Putting in reset and setting prescale _ retract_servo\n");
		// Set into sleep mode.
		I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
		I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);
		temp_reg |= (1 << 4); // Turn off oscillator
		temp_reg &= ~(1); // Turn off oscillator
		I2cWriteByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, temp_reg);
	} 

	// Set prescale clock to 0x83 for 46Hz PWM (21.7ms period)
	I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_PRESCALE_REG, 0x83);

	// Turn on oscillator
	I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);
	temp_reg &= ~(1 << 4); 
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
	// 	if((temp_reg & (PCA9685_HELD_RESET)) == 0){
	// 		on = 1;
	// 	}
	// }

	printf("Device is Running _ retract_servo\n");

	// Need to turn on the outputs for a 1.7ms on time for the 21.7ms period.
	// On Value = 0x0 (0,0)
	// Off Value = 0x141 *(1,41)
	//
	// -----
	//     |
	//      -----------------
	//    1.7               21.7
	//
	I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_ON_L, 0x0);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_ON_H, 0x0);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_OFF_L, 0x41);
	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_OFF_H, 0x1);

	printf("Retracting servo 1.3ms - sleeping _ retract_servo\n");
	// Wait two seconds (or however long the it takes to extend)
	//usleep(TWOSECONDS);

	printf("Waking up, shutting down servo _ retract_servo\n");

	// Turn off the outputs.
	// Can turn into a single write once verified.
//	I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
//	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_ON_L, 0x0);
//	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_ON_H, 0x0);
//	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_OFF_L, 0x0);
//	I2cWriteByte(PCA9685_I2C_BUS, PCA9685_SERVO_OFF_H, 0x0);

	// Terminate Bus
	PCA9685_End();

	return 0;
}

