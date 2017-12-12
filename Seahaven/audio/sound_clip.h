#ifndef __AUDIO_SOUND_CLIP_H__
#define __AUDIO_SOUND_CLIP_H__

#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

// sc_ is the SoundClip buffer of audio
// scb_ is the SoundClip buffer number of buff_sizes used
void*    sc_camera;
uint32_t scb_camera;
void*    sc_chat;
uint32_t scb_chat;
void*    sc_fire_off;
uint32_t scb_fire_off;
void*    sc_fire_on;
uint32_t scb_fire_on;
void*    sc_lights_off;
uint32_t scb_lights_off;
void*    sc_lights_on;
uint32_t scb_lights_on;
void*    sc_misunderstand;
uint32_t scb_misunderstand;
void*    sc_not_hot;
uint32_t scb_not_hot;
void*    sc_notification;
uint32_t scb_notification;
void*    sc_quick_maths;
uint32_t scb_quick_maths;
void*    sc_skraa;
uint32_t scb_skraa;
void*    sc_thud_couch;
uint32_t scb_thud_couch;
void*    sc_thud_truman;
uint32_t scb_thud_truman;
void*    sc_i_did_not;
uint32_t scb_i_did_not;

// This sets up PCM and also all the buffers
void soundClipSetup();

// free all buffers and cleans up PCM
void soundClipCleanup();

// This is used to play a sound of choice
void soundClipPlay(void* sc_file, uint32_t buffers);
  
#endif
