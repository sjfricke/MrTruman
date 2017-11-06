#include "PCA9685.h"
#include "hardware_tests.h"
#include "gpio.h"
#include "LSM6DS3H.h"

static void testLEDFanServo() {
    retractServo();
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
    extendServo();
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
    servoOff();
}

static void testGyro() {
    
    // turn on Gyro
    enableGyroTilt();

    // Gyro INT1 is on GPIO_E (pin 27 on dragonboard)
    //  Set GPIO_E dir as input
    //  Poll to determine when interrupted
    char* pin = GpioDB410cMapping(27);
    printf("using pin %s", pin);
    printf(" with curr direction %d", GpioGetDirection(pin)); 
    printf(" with GPIO val %d\n", GpioGetValue(pin));
    GpioEnablePin(pin);
    GpioSetDirection(pin, INPUT_PIN);

    printf("Now direction is %d", GpioGetDirection(pin)); 
    printf(" with GPIO val %d\n", GpioGetValue(pin));
    uint8_t data;
    // enable tilt event detection, two LSB conclude two ops (idk what they are) 
    I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_FUNC_SRC, &data);
    int interrupted = 0;
    while (!interrupted) {
        if (GpioGetValue(pin) > 0) {
            interrupted = 1;
        }
    }
    printf("GPIO val %d\n", GpioGetValue(pin));
    // clear 
    I2cReadByte(LSM6DS3H_I2C_BUS, LSM6DS3H_FUNC_SRC, &data);
    printf("FUNC_SRC reg is 0x%x", data);
    printf("\nGRYO INT1 DETECTED\n");
}

void hardwareTests() {
    testGyro();
    if (0) testLEDFanServo();
}

