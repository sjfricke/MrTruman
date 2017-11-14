#define ALSA_PCM_NEW_HW_PARAMS_API

#include <stdio.h>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include "../hardware/PCA9685.h"
#include "../hardware/gpio.h"

#define PCM_DEVICE "plughw:0,1"

#define RATE 	 = 44100;
#define CHANNELS = 1;
#define SECONDS  = 100;

// used to actually run music
// make sure to call loopbackSetup() first
int loopback();

// setup ALSA to run
void loopbackSetup();

// Really wont ever call this
void loopbackTerminate();
