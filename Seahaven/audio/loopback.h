#define ALSA_PCM_NEW_HW_PARAMS_API

#include <stdio.h>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include "../server/server.h"
#include "../hardware/PCA9685.h"
#include "../hardware/gpio.h"
#include "../status.h"

#define PCM_DEVICE "plughw:0,1"

#define RATE 44100
#define CHANNELS 1
#define SECONDS 100
#define BROADCASTCOLORS 1
#define SCALECOLORS 255

uint8_t VOLCHANGED;
uint8_t VOLCURRENT;

// used to actually run music
// make sure to call loopbackSetup() first
int loopback();

// setup ALSA to run
void loopbackSetup();

// Really wont ever call this
void loopbackTerminate();

static void loopInputSetup();
static void loopOutputSetup();
static void setupHandles();
