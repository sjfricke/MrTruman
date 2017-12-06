#include "status.h"
#include "server/server.h" 
#include "audio/voice.h"
#include "hardware/LSM6DS3H.h"
#include "hardware/PCA9685.h"
#include "audio/loopback.h"

#include <stdio.h>
#include <unistd.h>


// This function is used to send data from Web to Host
// Everything is assuming the README under the Seahaven folder
void webDataCallback( int type, char* value );

// side loop to constantly get temperature
// Should be ran on seperate thread
pthread_t tempThread;
void* pollTemperature( void* x );

// This is a void argument hardware setup
// This is for setting up things like componenets but will not give references
// This means you will need to still get a mapping of the GPIO pin for the main function
void HardwareSetup( void );

static void initAuxGPIO() {
    uint16_t pin = GpioDB410cMapping(23);
    GpioEnablePin(pin);
    GpioSetDirection(pin, INPUT_PIN);
}
