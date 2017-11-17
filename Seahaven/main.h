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

// This is a void argument hardware setup
// This is for setting up things like componenets but will not give references
// This means you will need to still get a mapping of the GPIO pin for the main function
void HardwareSetup( void );
