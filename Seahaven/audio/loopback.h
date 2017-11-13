#define ALSA_PCM_NEW_HW_PARAMS_API

#include <stdio.h>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include "../hardware/PCA9685.h"

#define PCM_DEVICE "default"

int loopback();

// setup ALSA to run
void loopbackSetup();

static char *device = "plughw:0,0";
static snd_pcm_t *handle;
static int verbose = 0;		/* snd_pcm_dump() */

/* hwparams and default settings */
#define HWPARAMS_FORMAT SND_PCM_FORMAT_S16_LE
#define HWPARAMS_CHANNELS 2
#define HWPARAMS_RATE 44100
#define HWPARAMS_PERIOD_FRAMES 1024

static struct {
	snd_pcm_format_t format;
	unsigned int channels;
	unsigned int rate;
	snd_pcm_uframes_t period_frames;
	snd_pcm_uframes_t buffer_frames;
} hwparams = {
.format = HWPARAMS_FORMAT,.channels = HWPARAMS_CHANNELS,.rate =
	    HWPARAMS_RATE,.period_frames = HWPARAMS_PERIOD_FRAMES};
static snd_pcm_stream_t stream = SND_PCM_STREAM_CAPTURE;

/* holds interleaved channel PCM period signal from H/W buffer */
static u_char *audiobuf = NULL;
/* holds deinterleaved channel PCM in separate & contiguous regions */ 
static float *chnldata = NULL;
/* raw capture PCM data for plotting program (e.g. "gnuplot(1)") IPC */
static char *raw_capture_data_file = NULL; /* shm file for raw dump */
static void *raw_capture_data_map = NULL; /* mmap ptr */

/* miscalleneous */
static int quiet_mode = 0;
#ifndef timersub
#define	timersub(a, b, result) \
do { \
	(result)->tv_sec = (a)->tv_sec - (b)->tv_sec; \
	(result)->tv_usec = (a)->tv_usec - (b)->tv_usec; \
	if ((result)->tv_usec < 0) { \
		--(result)->tv_sec; \
		(result)->tv_usec += 1000000; \
	} \
} while (0)
#endif
