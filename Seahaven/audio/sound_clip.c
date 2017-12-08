#include "sound_clip.h"
#include "loopback.h"

static void soundClipLoad(const char* file, void** sc_buff, uint32_t* scb_buff) {
  FILE* fp;
  uint32_t f_len;

  fp = fopen(file, "rb");
  if (fp == NULL) { printf("ERROR: opening %s\n", file); return;}
  fseek(fp, 0, SEEK_END);
  f_len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  *sc_buff = malloc(f_len + (pcm_buff_size - (f_len % pcm_buff_size)));
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
  loopOutputSetup();
  
  for(uint32_t i = 0; i < buffers; i++) {
  
    memcpy(pcm_buffer, sc_file + (pcm_buff_size * i), pcm_buff_size);

    if ((wr = snd_pcm_writei(outhandle, pcm_buffer, framesout)) == -EPIPE) {
      printf("XRUN.\n");
      snd_pcm_prepare(outhandle);
    } else if (wr < 0) {
      printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(wr));
    }
  }

  memset(pcm_buffer, 0, pcm_buff_size);
  
  snd_pcm_drop(outhandle);
  snd_pcm_close(outhandle);
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
