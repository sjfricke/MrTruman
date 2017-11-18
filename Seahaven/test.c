#include "main.h"

static void testLEDFanServo() {

    servoStart();
    servoRetract();
    fanOn(.99);
  
    initLEDs();
  
    int i;
    for(i = 0; i < 10; i++){
      setLED(PCA9685_ALL_CALL, .35, (1<<i));
      usleep(100000);
      setLED(PCA9685_ALL_CALL, 0, (1<<i));
      usleep(50000);
  
    }
  
    usleep(1000000);
    setLED(PCA9685_ALL_CALL, .35, 0x3FF);
    usleep(1000000);
    setLED(PCA9685_ALL_CALL, 0, 0x3FF);
  
    for(i = 0; i < 10; i++){
      setLED(PCA9685_RED_ADDRESS, .95, (1<<i));
      usleep(100000);
      setLED(PCA9685_RED_ADDRESS, 0, (1<<i));
      usleep(50000);
  
    }
  
    usleep(1000000);
    servoExtend();
    setLED(PCA9685_RED_ADDRESS, .85, 0x3FF);
    usleep(1000000);
    setLED(PCA9685_RED_ADDRESS, 0, 0x3FF);
   
  
  
    for(i = 0; i < 10; i++){
      setLED(PCA9685_BLUE_ADDRESS, .95, (1<<i));
      usleep(100000);
      setLED(PCA9685_BLUE_ADDRESS, 0, (1<<i));
      usleep(50000);
  
    }
  
    usleep(1000000);
    setLED(PCA9685_BLUE_ADDRESS, .65, 0x3FF);
    usleep(1000000);
    setLED(PCA9685_BLUE_ADDRESS, 0, 0x3FF);
  
  
  
    for(i = 0; i < 10; i++){
      setLED(PCA9685_GREEN_ADDRESS, .95, (1<<i));
      usleep(100000);
      setLED(PCA9685_GREEN_ADDRESS, 0, (1<<i));
      usleep(50000);
  
    }
  
    usleep(1000000);
    setLED(PCA9685_GREEN_ADDRESS, .65, 0x3FF);
    usleep(1000000);
    setLED(PCA9685_GREEN_ADDRESS, 0, 0x3FF);
  
  
  
  
    for(i = 0; i < 10; i++){
      setLED(PCA9685_GREEN_ADDRESS, .95, (1<<i)+(1<<(i+1)));
      setLED(PCA9685_BLUE_ADDRESS, .95, ( (1<<9)>>(i)) +((1<<9)>>(i+1)) );
      usleep(100000);
      setLED(PCA9685_GREEN_ADDRESS, 0, (1<<i)+(1<<(i+1)));
      setLED(PCA9685_BLUE_ADDRESS, 0, ( (1<<9)>>(i)) +((1<<9)>>(i+1)));
      usleep(50000);
  
    }
  
    usleep(1000000);
    setLED(PCA9685_GREEN_ADDRESS, .65, 0x3FF);
    usleep(1000000);
    setLED(PCA9685_GREEN_ADDRESS, 0, 0x3FF);
    fanOff();
    servoStop();
}

static void testGyro() {
    
    // turn on Gyro
    enableGyroTilt();


    // Gyro INT1 is on GPIO_E (pin 27 on dragonboard)
    //  Set GPIO_E dir as input
    //  Poll to determine when interrupted
    //
    //
    //
       //printf("using pin %s", pin);
    //printf(" with curr direction %d", GpioGetDirection(pin)); 
    //printf(" with GPIO val %d\n", GpioGetValue(pin));
    //GpioEnablePin(pin);
    //GpioSetDirection(pin, 0);

    //printf("Now direction is %d", GpioGetDirection(pin)); 
    //printf(" with GPIO val %d\n", GpioGetValue(pin));

    uint8_t data;

    uint16_t interrupt_pin;
    
    // enable tilt event detection, two LSB conclude two ops (idk what they are) 
    //I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_FUNC_SRC, &data);
    //printf("data of interrupt status: %x\n", data);
    int interrupted = 0;

    interrupt_pin = GpioDB410cMapping(GYRO_INTERRUPT_PIN);

    while (interrupted < 10) {
	    data = GpioGetValue(interrupt_pin);
        if (data == 0) {
	    printf("Detected int \n");
            interrupted++;
    	    I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_FUNC_SRC, &data);
	    printf("data at interrupt %x\n", data);
	    usleep(1000000);
        }
    }

    //I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_FUNC_SRC, &data);
    //printf("data of interrupt status: %x\n", data);
    //printf("GPIO val %d\n", GpioGetValue(pin));
    // clear 
    I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_FUNC_SRC, &data);
    //printf("FUNC_SRC reg is 0x%x", data);
    //printf("\nGRYO INT1 DETECTED\n");
}

static int testCamera() {
  printf("\nTesting Camera\n");
}

int main ( int argc, char* argv[] ) {
  printf("test\n");
  //testGyro();
  //testLEDFanServo();
  //testCamera();
  initAuxGPIO();
  loopbackSetup();
  loopback();
  //
  //
  //servoStart();
  //servoExtend();
}