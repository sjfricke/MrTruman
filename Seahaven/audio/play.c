#include <alsa/asoundlib.h>
#include <stdio.h>

#define PCM_DEVICE "default"

unsigned int pcm, tmp, dir;
int rate, channels;
snd_pcm_t *pcm_handle;
snd_pcm_hw_params_t *params;
snd_pcm_uframes_t frames;
char *buff;
int buff_size, loops;
snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
ssize_t count;

FILE* fp;
void* a_lights_on;
void* a_lights_off;
void* a_skraa;
int b_lights_on;
int b_lights_off;
int b_skraa;

void setup() {
  int f_len;
  
  rate 	 = 44100;
  channels = 2;
  
  /* Open the PCM device in playback mode */
  if (pcm = snd_pcm_open(&pcm_handle, "plughw:0,1",
			 SND_PCM_STREAM_PLAYBACK, 0) < 0) 
    printf("ERROR: Can't open \"%s\" PCM device. %s\n",
	   PCM_DEVICE, snd_strerror(pcm));

  /* Allocate parameters object and fill it with default values*/
  snd_pcm_hw_params_alloca(&params);

  snd_pcm_hw_params_any(pcm_handle, params);

  /* Set parameters */
  if (pcm = snd_pcm_hw_params_set_access(pcm_handle, params,
					 SND_PCM_ACCESS_RW_INTERLEAVED) < 0) 
    printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(pcm));

  if (pcm = snd_pcm_hw_params_set_format(pcm_handle, params, format) < 0) 
    printf("ERROR: Can't set format. %s\n", snd_strerror(pcm));

  if (pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, channels) < 0) 
    printf("ERROR: Can't set channels number. %s\n", snd_strerror(pcm));

  if (pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0) < 0) 
    printf("ERROR: Can't set rate. %s\n", snd_strerror(pcm));

  if (pcm = snd_pcm_format_size(format, channels * rate) < 0)
    printf("ERROR: Can't set count %s\n", snd_strerror(pcm));
  
  /* Write parameters */
  if (pcm = snd_pcm_hw_params(pcm_handle, params) < 0)
    printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(pcm));

  /* Resume information */
  printf("PCM name: '%s'\n", snd_pcm_name(pcm_handle));

  printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(pcm_handle)));

  snd_pcm_hw_params_get_channels(params, &tmp);
  printf("channels: %i ", tmp);

  if (tmp == 1)
    printf("(mono)\n");
  else if (tmp == 2)
    printf("(stereo)\n");

  snd_pcm_hw_params_get_rate(params, &tmp, 0);
  printf("rate: %d bps\n", tmp);

  /* Allocate buffer to hold single period */
  snd_pcm_hw_params_get_period_size(params, &frames, 0);

  buff_size = frames * channels * 2 /* 2 -> sample size */;
  buff = (char *) malloc(buff_size);

  snd_pcm_hw_params_get_period_time(params, &tmp, NULL);
  
  // Read in files
  fp = fopen("audio/sounds/truman_lights_on.wav", "rb");
  if (fp == NULL) { printf("error - opening lights on\n"); exit(1); }
  fseek(fp, 0, SEEK_END);
  f_len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  a_lights_on = malloc(f_len + (buff_size - (f_len % buff_size)));
  if (fp == NULL) { printf("error - malloc  lights on\n"); exit(1); }
  b_lights_on = (f_len + (buff_size - (f_len % buff_size))) / buff_size;
  fread(a_lights_on, f_len, 1, fp);
  fclose(fp);

  fp = fopen("audio/sounds/truman_lights_off.wav", "rb");
  if (fp == NULL) { printf("error - opening lights off\n"); exit(1); }
  fseek(fp, 0, SEEK_END);
  f_len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  a_lights_off = malloc(f_len + (buff_size - (f_len % buff_size)));
  if (fp == NULL) { printf("error - malloc  lights off\n"); exit(1); }
  b_lights_off = (f_len + (buff_size - (f_len % buff_size))) / buff_size;
  fread(a_lights_off, f_len, 1, fp);
  fclose(fp);
  
  fp = fopen("audio/sounds/truman_skraa.wav", "rb");
  if (fp == NULL) { printf("error - opening skraa\n"); exit(1); }
  fseek(fp, 0, SEEK_END);
  f_len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  a_skraa = malloc(f_len + (buff_size - (f_len % buff_size)));
  if (fp == NULL) { printf("error - malloc  skraa\n"); exit(1); }
  b_skraa = (f_len + (buff_size - (f_len % buff_size))) / buff_size;
  printf("skraa Byte malloc: %d\n", f_len + (buff_size - (f_len % buff_size)));
  printf("skraa f_len: %d\n", f_len);
  printf("buff_size: %d\n", buff_size);
  printf("b_skraa: %d\n", b_skraa);
  fread(a_skraa, f_len, 1, fp);
  fclose(fp);

  return;
}

void play(void* a_file, int buffers) {

  for(int i = 0; i < buffers; i++) {

    memcpy(buff, a_file + (buff_size * i), buff_size);

    if (pcm = snd_pcm_writei(pcm_handle, buff, frames) == -EPIPE) {
      printf("XRUN.\n");
      snd_pcm_prepare(pcm_handle);
    } else if (pcm < 0) {
      printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
    }

  }

  return;
}

void cleanup() {
  snd_pcm_drain(pcm_handle);
  snd_pcm_close(pcm_handle);
  free(buff);
  free(a_lights_on);
  free(a_lights_off);
  free(a_skraa);
}

int main(int argc, char **argv) {
  setup();
  play(a_lights_on, b_lights_on);
  play(a_lights_off, b_lights_off);
  play(a_skraa, b_skraa);
  cleanup();
  return 0;
}
