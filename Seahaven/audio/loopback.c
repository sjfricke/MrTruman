/*

   This example reads from the default PCM device
   and writes to standard output for 5 seconds of data.

*/

/* Use the newer ALSA API */
#include "loopback.h"

snd_pcm_t *inhandle;
snd_pcm_t *outhandle;
snd_pcm_uframes_t frames, framesout;
int aux_in = 1;
static uint16_t pin;
void *buffer;
int buff_size;


static void setupHandles() {
	loopInputSetup();
	loopOutputSetup();
}

static void closeHandles() {
	snd_pcm_drain(inhandle);
	snd_pcm_close(inhandle);
	snd_pcm_drain(outhandle);
	snd_pcm_close(outhandle);
}

void loopbackSetup() {
	initLEDs();
	setupHandles();
	buffer = (void *) malloc(buff_size);
}

void loopbackTerminate() {
	closeHandles();
	free(buffer);
}

static int get_max(int16_t* buffer) {
	int16_t max = 0;
	int16_t curr;
	for (int i = 0; i < 1500; i++) {

		curr = buffer[i];
		if (curr > max) {
			// for some reason each buffer ends in 18161
			max = curr == 18161 ? max : curr;
		}
	}
	return max;
}

// Returns 1 if we should reset the LEDs
static int change_LED_from_sample(int16_t sample, int scale) {
	if (sample >= scale && sample < 2*scale) {
		setLED(PCA9685_RED_ADDRESS, .99, 0x3ff);
		setLED(PCA9685_BLUE_ADDRESS, 0, 0x3ff);
		setLED(PCA9685_GREEN_ADDRESS, 0, 0x3ff);
	} else if (sample >= 2*scale && sample < 3*scale) {
		setLED(PCA9685_RED_ADDRESS, .99, 0x3ff);
		setLED(PCA9685_GREEN_ADDRESS, .5, 0x3ff);
		setLED(PCA9685_BLUE_ADDRESS, 0, 0x3ff);
	} else if (sample >= 3*scale && sample < 4*scale) {
		setLED(PCA9685_GREEN_ADDRESS, .5, 0x3ff);
		setLED(PCA9685_BLUE_ADDRESS, 0, 0x3ff);
		setLED(PCA9685_RED_ADDRESS, 0, 0x3ff);
	} else if (sample >= 4*scale && sample < 5*scale) {
		setLED(PCA9685_GREEN_ADDRESS, .5, 0x3ff);
		setLED(PCA9685_BLUE_ADDRESS, .5, 0x3ff);
		setLED(PCA9685_RED_ADDRESS, 0, 0x3ff);
	} else if (sample >= 5*scale && sample < 6*scale) {
		setLED(PCA9685_BLUE_ADDRESS, .5, 0x3ff);
		setLED(PCA9685_GREEN_ADDRESS, 0, 0x3ff);
		setLED(PCA9685_RED_ADDRESS, 0, 0x3ff);
	} else if (sample >= 6*scale && sample < 7*scale) {
		setLED(PCA9685_BLUE_ADDRESS, .5, 0x3ff);
		setLED(PCA9685_RED_ADDRESS, .99, 0x3ff);
		setLED(PCA9685_GREEN_ADDRESS, 0, 0x3ff);
	} else if (sample >= 7*scale) {
		setLED(PCA9685_BLUE_ADDRESS, .5, 0x3ff);
		setLED(PCA9685_RED_ADDRESS, .99, 0x3ff);
		setLED(PCA9685_GREEN_ADDRESS, .5, 0x3ff);
	} else {
		return 1;
	}
	return 0;
}

static void turn_off_all_LEDs() {
	setLED(PCA9685_RED_ADDRESS, 0, 0x3ff);
	setLED(PCA9685_BLUE_ADDRESS, 0, 0x3ff);
	setLED(PCA9685_GREEN_ADDRESS, 0, 0x3ff);
}

// Function that thread runs through until Aux unplugged
void *analyze_buffer(void* buff) {
	int16_t* buffer = (int16_t*) buff;
	int base_scale = 100;
	int scale = 700;
	int comp = 0;
	uint8_t count = 1;
	uint8_t rst = 0;

	while ((aux_in = GpioGetValue(pin)) == 1) {
		// get max ampl in buffer
		int16_t sample = get_max(buffer);
		// change LEDs based on the sample and scale
		rst = change_LED_from_sample(sample, scale);

		// see if we should change the scale to make the LED
		//  feedback more interesting
		//  we dont want to cap out
		if (sample >= 8*scale) {
			// increase scale if we get loud
			scale += 100;
		} else if (sample < scale && (scale-100) >= base_scale) {
			// decrease scale if the song is quiet
			scale -= 100;
		}

		// flash LEDs for large amplitude changes
		if (comp/2 > sample || comp*2 > sample) {
			// rst for big changes
			rst = 1;
			comp = sample;
		}

		if (rst) {
			turn_off_all_LEDs();
		}
	}
	// turn all off before leaving
	turn_off_all_LEDs();
}
static void loopOutputSetup() {

	//////////////////////////////////////////////////
	///////////////// OUTPUT SETUP ///////////////////
	//////////////////////////////////////////////////

	unsigned int pcm, tmp;
	int rate, channels, seconds;
	snd_pcm_hw_params_t *paramsout;

	rate = RATE;
	channels = CHANNELS;
	seconds = SECONDS;

	/* Open the PCM device in playback mode */
	if (pcm = snd_pcm_open(&outhandle, PCM_DEVICE,
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

	buff_size = frames * channels * 2;   /* 2 bytes/sample, 2 channels */
	printf("size is %d\n", buff_size);

}

static void loopInputSetup() {
	//////////////////////////////////////////////////
	/////////////// INPUT SETUP //////////////////////
	//////////////////////////////////////////////////

	int rc;
	snd_pcm_hw_params_t *params;
	unsigned int val;
	int dir;

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
	val = RATE;
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

	/* We want to loop for 5 seconds */
	snd_pcm_hw_params_get_period_time(params,
			&val, &dir);
}

int loopback() {
	pin = GpioDB410cMapping(23);
	pthread_t tid;
	int wr=0;

	pthread_create(&tid, NULL, analyze_buffer, (void *)buffer);
	// loop the entire time the aux cord is plugged in
	while (aux_in == 1) {
		snd_pcm_readi(inhandle, buffer, frames);		
		wr = snd_pcm_writei(outhandle, buffer, framesout);	
		if (wr < 0) {
			printf("WRITE ERR %s\n", snd_strerror(wr));
			//snd_pcm_recover(outhandle, wr, 0);
			closeHandles();
			setupHandles();
			printf("Re initialized: Lets retry that\n");
		}
	}

	printf("Aux unplugged, see ya next time\n");
	pthread_join(tid, NULL);
	return 0;
}




