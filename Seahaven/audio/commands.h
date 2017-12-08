#ifndef __AUDIO_COMMANDS_H__
#define __AUDIO_COMMANDS_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>

// TODO not need a ../ path
#include "../server/server.h"
#include "../status.h"
#include "sound_clip.h"

// Assuming values match with int index of animation spec
typedef enum {
  LIGHT_SWTICH,
  LIGHT_COLOR,
  FIREPLACE,
  CAMERA,
  SEAKERS,
  MUSIC,
  GYRO
} anim_type;

// return -1 if invalid command
int commandDetect(char const* voice_str);

#endif
