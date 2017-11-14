#define ALSA_PCM_NEW_HW_PARAMS_API

#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include "../hardware/PCA9685.h"
#include "../hardware/gpio.h"

#define PCM_DEVICE "plughw:0,1"

#define RATE 	  44100
#define CHANNELS  1
#define SECONDS   100

// used to actually run music
// make sure to call loopbackSetup() first
int loopback();

// setup ALSA to run
void loopbackSetup();

// Really wont ever call this
void loopbackTerminate();

void fftTest();


#define SIN_2PI_16 0.38268343236508978
#define SIN_4PI_16 0.707106781186547460
#define SIN_6PI_16 0.923879532511286740
#define C_P_S_2PI_16 1.30656296487637660
#define C_M_S_2PI_16 0.54119610014619690
#define C_P_S_6PI_16 1.3065629648763766
#define C_M_S_6PI_16 -0.54119610014619690
