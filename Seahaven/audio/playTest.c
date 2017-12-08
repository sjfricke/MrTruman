#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#define PCM_DEVICE "default"

static uint32_t pcm;
static uint32_t tmp;
static uint32_t rate = 44100; // hard assumption
static uint8_t channels = 2; // stero
static snd_pcm_t *pcm_handle;
static snd_pcm_hw_params_t *params;
static snd_pcm_uframes_t frames;
static char *buff;
static uint32_t buff_size;
static snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

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

static void soundClipLoad(const char* file, void** sc_buff, uint32_t* scb_buff) {
  FILE* fp;
  uint32_t f_len;

  fp = fopen(file, "rb");
  if (fp == NULL) { printf("ERROR: opening %s\n", file); return;}
  fseek(fp, 0, SEEK_END);
  f_len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  *sc_buff = malloc(f_len + (buff_size - (f_len % buff_size)));
  if (sc_buff == NULL) {
    printf("ERROR: malloc %s\n", file);
    fclose(fp);
    return;
  }
  *scb_buff = (f_len + (buff_size - (f_len % buff_size))) / buff_size;
  printf("[%d] [%d] [%d] [%d]\n", f_len, buff_size,(buff_size - (f_len % buff_size)), *scb_buff); 
  fread(*sc_buff, f_len, 1, fp);

  fclose(fp);
}

static void soundClipPCMSetup() {
   
  /* Open the PCM device in playback mode */
  if ((pcm = snd_pcm_open(&pcm_handle, "plughw:0,1", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
    printf("ERROR: Can't open \"%s\" PCM device. %s\n", PCM_DEVICE, snd_strerror(pcm));
  }

  /* Allocate parameters object and fill it with default values*/
  snd_pcm_hw_params_alloca(&params);
  snd_pcm_hw_params_any(pcm_handle, params);

  /* Set parameters */
  if ((pcm = snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
    printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(pcm));
  }

  if ((pcm = snd_pcm_hw_params_set_format(pcm_handle, params, format)) < 0) {
    printf("ERROR: Can't set format. %s\n", snd_strerror(pcm));
  }

  if ((pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, channels)) < 0) {
    printf("ERROR: Can't set channels number. %s\n", snd_strerror(pcm));
  }

  if ((pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0)) < 0) {
    printf("ERROR: Can't set rate. %s\n", snd_strerror(pcm));
  }

  if ((pcm = snd_pcm_format_size(format, channels * rate)) < 0) {
    printf("ERROR: Can't set count %s\n", snd_strerror(pcm));
  }
  
  /* Write parameters */
  if ((pcm = snd_pcm_hw_params(pcm_handle, params)) < 0) {
    printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(pcm));
  }
  
  /* Allocate buffer to hold single period */
  snd_pcm_hw_params_get_period_size(params, &frames, 0);

  buff_size = frames * channels * 2 /* 2 -> sample size */;
  buff = (char *) malloc(buff_size);
  if (buff == NULL){printf("ERROR: malloc buff\n");}
  snd_pcm_hw_params_get_period_time(params, &tmp, NULL);
    
  return;
}

static void soundClipPlay(void* sc_file, uint32_t buffers) {
  
  for(uint32_t i = 0; i < buffers; i++) {
  
    memcpy(buff, sc_file + (buff_size * i), buff_size);

    if ((pcm = snd_pcm_writei(pcm_handle, buff, frames)) == -EPIPE) {
      printf("XRUN.\n");
      snd_pcm_prepare(pcm_handle);
    } else if (pcm < 0) {
      printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
    }
  }
  return;
}

void soundClipCleanup() {
  snd_pcm_drain(pcm_handle);
  snd_pcm_close(pcm_handle);
  free(buff);

  free(sc_camera);
  free(sc_chat);
  free(sc_fire_off);
  free(sc_fire_on);
  free(sc_lights_off);
  free(sc_lights_on);
  free(sc_misunderstand);
  free(sc_not_hot);
  free(sc_notification);
  free(sc_quick_maths);
  free(sc_skraa);
  free(sc_thud_couch);
  free(sc_thud_truman);
}

int main(int argc, char **argv) {
  soundClipPCMSetup();
  
  // Read in files
  soundClipLoad("audio/sounds/truman_camera.wav",       &sc_camera, &scb_camera);
  soundClipLoad("audio/sounds/truman_chat.wav",         &sc_chat, &scb_chat);
  soundClipLoad("audio/sounds/truman_fire_off.wav",     &sc_fire_off, &scb_fire_off);
  soundClipLoad("audio/sounds/truman_fire_on.wav",      &sc_fire_on, &scb_fire_on);
  soundClipLoad("audio/sounds/truman_lights_off.wav",   &sc_lights_off, &scb_lights_off);
  soundClipLoad("audio/sounds/truman_lights_on.wav",    &sc_lights_on, &scb_lights_on); 
  soundClipLoad("audio/sounds/truman_misunderstand.wav",&sc_misunderstand, &scb_misunderstand);
  soundClipLoad("audio/sounds/truman_not_hot.wav",      &sc_not_hot, &scb_not_hot);
  soundClipLoad("audio/sounds/truman_notification.wav", &sc_notification, &scb_notification);
  soundClipLoad("audio/sounds/truman_quick_maths.wav",  &sc_quick_maths, &scb_quick_maths);
  soundClipLoad("audio/sounds/truman_skraa.wav",        &sc_skraa, &scb_skraa);
  soundClipLoad("audio/sounds/truman_thud_couch.wav",   &sc_thud_couch, &scb_thud_couch);
  soundClipLoad("audio/sounds/truman_thud_truman.wav",  &sc_thud_truman, &scb_thud_truman);
  
  // play files
  usleep(1000);
  //  soundClipPlay(sc_thud_couch, scb_thud_couch);
  //usleep(10000);
  soundClipPlay(sc_camera, scb_camera);
  usleep(100000);
  soundClipCleanup();
  return 0;
}
