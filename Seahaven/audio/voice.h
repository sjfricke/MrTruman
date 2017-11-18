#ifndef __AUDIO_VOICE_H__
#define __AUDIO_VOICE_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <sys/select.h>

#include <sphinxbase/err.h>
#include <sphinxbase/ad.h>
#include <pocketsphinx.h>

#include "commands.h"
#include "../hardware/LSM6DS3H.h"
#include "../hardware/gpio.h"
#include "../status.h"
#define PS_BUF_SIZE 4096
#define PS_BUF_TIME 15

#define PS_TRUMAN_LM "/root/MrTruman/Seahaven/audio/ps_truman.lm"
#define PS_TRUMAN_DICT "/root/MrTruman/Seahaven/audio/ps_truman.dict"

//#define PS_TRUMAN_LM "/usr/local/share/pocketsphinx/model/en-us/en-us.lm.bin"
//#define PS_TRUMAN_DICT "/usr/local/share/pocketsphinx/model/en-us/cmudict-en-us.dict"

#define PS_ALSA_VOICE_MIC_HW "plughw:0,2"

void voiceHardwareSetup();

void voiceDictionarySetup();

// return -1 if invalid command
int voiceCommand();

void voiceCleanUp();
  
#endif
