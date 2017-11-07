#include "server/server.h" 
#include "audio/voice.h"
#include "hardware/LSM6DS3H.h"
#include "hardware/PCA9685.h"

#include <stdio.h>
#include <unistd.h>


void webData( int type, char* value);

void animationStatus( char* status);

void updateLED( char* rgb );

void takePhoto( void );
