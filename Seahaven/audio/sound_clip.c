#include "sound_clip.h"
#include <stdlib.h>

int pcm_buff_size = 7526;
snd_pcm_uframes_t frames, framesout;

void *  pcm_buffer[7526];


static void soundClipLoad(const char* file, void** sc_buff, uint32_t* scb_buff) {
  FILE* fp;
  uint32_t f_len;

  fp = fopen(file, "rb");
  if (fp == NULL) { printf("ERROR: opening %s\n", file); return;}
  fseek(fp, 0, SEEK_END);
  f_len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  *sc_buff = calloc(f_len + pcm_buff_size - (f_len % pcm_buff_size), sizeof(char));
  if (sc_buff == NULL) {
    printf("ERROR: malloc %s\n", file);
    fclose(fp);
    return;
  }
  *scb_buff = (f_len + (pcm_buff_size - (f_len % pcm_buff_size))) / pcm_buff_size;
  printf("[%d] [%d] [%d] [%d]\n", f_len, pcm_buff_size,(pcm_buff_size - (f_len % pcm_buff_size)), *scb_buff); 
  fread(*sc_buff, f_len, 1, fp);

  fclose(fp);
}

void soundClipPlay(void* sc_file, uint32_t buffers) {
  int wr = 0;

  unsigned int pcm, tmp;
	snd_pcm_hw_params_t *paramsout;
  snd_pcm_t *outhandle;

	int rate = 44100;
	int channels = 1;

	/* Open the PCM device in playback mode */
	if (pcm = snd_pcm_open(&outhandle, "plughw:0,1",
				SND_PCM_STREAM_PLAYBACK, 0) < 0) 
		printf("ERROR: Can't open \"%s\" PCM device. %s\n",
				"plughw:0,1", snd_strerror(pcm));

	/* Allocate parameters object and fill it with default values*/
	snd_pcm_hw_params_alloca(&paramsout);
	snd_pcm_hw_params_any(outhandle, paramsout);

	/* Set parameters */
	if (pcm = snd_pcm_hw_params_set_access(outhandle, paramsout,
				SND_PCM_ACCESS_RW_INTERLEAVED) < 0) 
		printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_format(outhandle, paramsout,
				SND_PCM_FORMAT_S16_LE) < 0) 
		printf("ERROR: Can't set format. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_channels(outhandle, paramsout, channels) < 0) 
		printf("ERROR: Can't set channels number. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_rate_near(outhandle, paramsout, &rate, 0) < 0) 
		printf("ERROR: Can't set rate. %s\n", snd_strerror(pcm));

	/* Write parameters */
	if (pcm = snd_pcm_hw_params(outhandle, paramsout) < 0)
		printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(pcm));

	/* Resume information */
	snd_pcm_hw_params_get_channels(paramsout, &tmp);
	snd_pcm_hw_params_get_rate(paramsout, &tmp, 0);

	/* Allocate buffer to hold single period */
	snd_pcm_hw_params_get_period_size(paramsout, &framesout, 0);
	snd_pcm_hw_params_get_period_time(paramsout, &tmp, NULL);
  
  for(uint32_t i = 0; i < buffers; i++) {
  
    memset(pcm_buffer, 0, pcm_buff_size);
    memcpy(pcm_buffer, sc_file + (pcm_buff_size * i), pcm_buff_size);

    wr = snd_pcm_writei(outhandle, pcm_buffer, framesout);
     if ((wr < 0) || (wr == -EPIPE)) {
//	     snd_pcm_drop(outhandle);
//	     snd_pcm_prepare(outhandle);
      printf("WRITE ERR %s\n", snd_strerror(wr));
    }
  }

  //usleep(4000);
  //snd_pcm_drop(outhandle);

  usleep(4000);
  snd_pcm_drop(outhandle);
  snd_pcm_close(outhandle);
 // free(pcm_buffer);
}
 
void soundClipCleanup() {
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

void soundClipSetup() {

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

}
