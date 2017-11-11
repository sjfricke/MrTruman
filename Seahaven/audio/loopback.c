/*

   This example reads from the default PCM device
   and writes to standard output for 5 seconds of data.

 */

/* Use the newer ALSA API */
#include "loopback.h"
int analyze_buffer(char* buffer, int len) {
    if (buffer[0] == '5') {
        setLED(ALL_CALL_RED_ADDRESS, .5, 0x3ff);
    }
    else if (buffer[1] == '5') {
        setLED(ALL_CALL_BLUE_ADDRESS, .5, 0x3ff);
    }
    else if (buffer[2] == '5') {
        setLED(ALL_CALL_GREEN_ADDRESS, .5, 0x3ff);
    }
    else if (buffer[0] == '3') {
        setLED(ALL_CALL_RED_ADDRESS, .5, 0x3ff);
    }
    else if (buffer[1] == '3') {
        setLED(ALL_CALL_BLUE_ADDRESS, .5, 0x3ff);
    }
    else if (buffer[2] == '3') {
        setLED(ALL_CALL_GREEN_ADDRESS, .5, 0x3ff);
    }
}

int loopback() {

	/////////////// INPUT SETUP

	long loops;
	int rc;
	int size;
	snd_pcm_t *inhandle;
	snd_pcm_hw_params_t *params;
	unsigned int val;
	int dir;
	snd_pcm_uframes_t frames;
	char *buffer;


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

	///////////////// OUTPUT SETUP


	unsigned int pcm, tmp;
	int rate, channels, seconds;
	snd_pcm_t *outhandle;
	snd_pcm_hw_params_t *paramsout;
	snd_pcm_uframes_t framesout;
	char *buff;
	int buff_size;
	//loops;

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
	printf("PCM name: '%s'\n", snd_pcm_name(outhandle));

	printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(outhandle)));

	snd_pcm_hw_params_get_channels(paramsout, &tmp);
	printf("channels: %i ", tmp);

	snd_pcm_hw_params_get_rate(paramsout, &tmp, 0);
	printf("rate: %d bps\n", tmp);

	printf("seconds: %d\n", seconds);	


	/* Allocate buffer to hold single period */
	snd_pcm_hw_params_get_period_size(paramsout, &framesout, 0);

	printf("FRAMES: %d\n",framesout);

	buff_size = framesout * channels * 2 /* 2 -> sample size */;
	buff = (char *) malloc(buff_size);

	snd_pcm_hw_params_get_period_time(paramsout, &tmp, NULL);

	////////////////////////////////////////////////

	printf("Other frames: %d\n",frames);

    size = frames * channels * 2;   /* 2 bytes/sample, 2 channels */
    printf("size is %d\n", size);
	buffer = (char *) malloc(size);

	/* We want to loop for 5 seconds */
	snd_pcm_hw_params_get_period_time(params,
			&val, &dir);
	loops = 5000000 / val;

	while (1) {
		snd_pcm_readi(inhandle, buffer, (frames));
        analyze_buffer(buffer, size);
		snd_pcm_writei(outhandle, buffer, framesout);
	}

	snd_pcm_drain(inhandle);
	snd_pcm_close(inhandle);
	free(buffer);


	snd_pcm_drain(outhandle);
	snd_pcm_close(outhandle);
	free(buff);

	return 0;
}



