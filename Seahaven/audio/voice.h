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
#define PS_BUF_SIZE 2048
#define PS_BUF_TIME 15

void voiceDictionarySetup();

// return -1 if invalid command
int voiceCommand();

void voiceCleanUp();
  
#endif
