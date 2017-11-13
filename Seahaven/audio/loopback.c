/*

   This example reads from the default PCM device
   and writes to standard output for 5 seconds of data.

*/

/* Use the newer ALSA API */
#include <pthread.h>

#include "loopback.h"

static int buff_size;

void *analyze_buffer(void* buff) {
	int16_t* buffer = (int16_t*) buff;
	int red_count, blue_count, green_count;
	uint8_t count = 0;
	while (1) {
		// get current buffer before it is modified again by reading/writing
		int16_t sample = buffer[0];
		
		red_count = 0;
		blue_count = 0;
		green_count = 0;
		if (sample > 200 && sample < 300) {
			setLED(PCA9685_RED_ADDRESS, .99, 0x3ff);
		}
		else {
			setLED(PCA9685_RED_ADDRESS, 0, 0x3ff);
		}

		if (sample > 300 && sample < 400) {
			setLED(PCA9685_GREEN_ADDRESS, .5, 0x3ff);
		}
		else {
			setLED(PCA9685_GREEN_ADDRESS, 0, 0x3ff);
		}

		if (sample > 400) {
			setLED(PCA9685_BLUE_ADDRESS, .5, 0x3ff);
		}
		else {
			setLED(PCA9685_BLUE_ADDRESS, 0, 0x3ff);
		}
		// sleep for .1 sec
		//usleep(100);
	}
}

int loopback() {

	//////////////////////////////////////////////////
	/////////////// INPUT SETUP //////////////////////
	//////////////////////////////////////////////////

	int size, rc;
	snd_pcm_t *inhandle;
	snd_pcm_hw_params_t *params;
	unsigned int val;
	int dir;
	snd_pcm_uframes_t frames;
	char *buffer;

	initLEDs();

	/* Open PCM device for recording (capture). */
	rc = snd_pcm_open(&inhandle, "plughw:0,2",
			SND_PCM_STREAM_CAPTURE, 0);

	if (rc < 0) {
		fprintf(stderr,
				"unable to open pcm device: %s\n",
				snd_strerror(rc));
		exit(1);
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(&params);

	/* Fill it in with default values. */
	snd_pcm_hw_params_any(inhandle, params);

	/* Set the desired hardware parameters. */

	/* Interleaved mode */
	snd_pcm_hw_params_set_access(inhandle, params,
			SND_PCM_ACCESS_RW_INTERLEAVED);

	/* Signed 16-bit little-endian format */
	snd_pcm_hw_params_set_format(inhandle, params,
			SND_PCM_FORMAT_S16_LE);


	/* Two channels (stereo) */
	snd_pcm_hw_params_set_channels(inhandle, params, 1);

	/* 44100 bits/second sampling rate (CD quality) */
	val = 44100;
	snd_pcm_hw_params_set_rate_near(inhandle, params,
			&val, &dir);


	/* Set period size to 32 frames. */
	frames = 32;
	snd_pcm_hw_params_set_period_size_near(inhandle,
			params, &frames, &dir);

	/* Write the parameters to the driver */
	rc = snd_pcm_hw_params(inhandle, params);
	if (rc < 0) {
		fprintf(stderr,
				"unable MEK to set hw parameters: %s\n",
				snd_strerror(rc));
		exit(1);
	}

	/* Use a buffer large enough to hold one period */
	snd_pcm_hw_params_get_period_size(params,
			&frames, 0);

	//////////////////////////////////////////////////
	///////////////// OUTPUT SETUP ///////////////////
	//////////////////////////////////////////////////

	unsigned int pcm, tmp;
	int rate, channels, seconds;
	snd_pcm_t *outhandle;
	snd_pcm_hw_params_t *paramsout;
	snd_pcm_uframes_t framesout;
	int buff_size;

	rate 	 = 44100;
	channels = 1;
	seconds  = 100;

	/* Open the PCM device in playback mode */
	if (pcm = snd_pcm_open(&outhandle, "plughw:0,1",
				SND_PCM_STREAM_PLAYBACK, 0) < 0) 
		printf("ERROR: Can't open \"%s\" PCM device. %s\n",
				PCM_DEVICE, snd_strerror(pcm));

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

	////////////////////////////////////////////////
	printf("PCM name: '%s'\n", snd_pcm_name(outhandle));
	printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(outhandle)));
	printf("rate: %d bps\n", tmp);
	printf("seconds: %d\n", seconds);	
	printf("channels: %i ", tmp);
	printf("FRAMES: %d\n",framesout);
	printf("Other frames: %d\n",frames);

	size = frames * channels * 2;   /* 2 bytes/sample, 2 channels */
	buff_size = size;
	printf("size is %d\n", size);
	buffer = (char *) malloc(size);

	/* We want to loop for 5 seconds */
	snd_pcm_hw_params_get_period_time(params,
			&val, &dir);

	pthread_t tid;
	pthread_create(&tid, NULL, analyze_buffer, (void *)buffer);

	while (1) {
		snd_pcm_readi(inhandle, buffer, frames);	
	        snd_pcm_writei(outhandle, buffer, framesout);	
	}
	pthread_join(tid, NULL);
	snd_pcm_drain(inhandle);
	snd_pcm_close(inhandle);
	snd_pcm_drain(outhandle);
	snd_pcm_close(outhandle);
	free(buffer);

	return 0;
}




