#include "PCA9685.h"

void hardware_tests() {
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