/*

   This example reads from the default PCM device
   and writes to standard output for 5 seconds of data.

 */

/* Use the newer ALSA API */
#include "pthread.h"

#include "loopback.h"
static inline void deinterleave(void)
{
	int i, j, k, chnls = hwparams.channels;
	float *dst = chnldata;
	uint8_t *src = audiobuf, *ptr;
	int32_t psize = hwparams.period_frames;
	snd_pcm_format_t format = hwparams.format;
	int fmt_nominal_width_bits = snd_pcm_format_width(format);
	int fmt_nominal_width_bytes = fmt_nominal_width_bits / 8;
	int fmt_phys_width_bits = snd_pcm_format_physical_width(format);
	int fmt_phys_width_bytes = fmt_phys_width_bits / 8;
	union {
		int32_t i;
		uint32_t u;
	} resln;

	for (i = 0; i < psize; i++) {
		ptr = src + (i * fmt_phys_width_bytes * chnls);
		for (j = 0; j < chnls; j++) {

			/* to support variety of sample formats, perform byte-by-byte
			   extraction for each sample word */
			ptr += j * fmt_phys_width_bytes;
			for (resln.u &= 0x0, k = 0; k < fmt_phys_width_bytes;
			     k++) {
				/* handle endianess of current sample format */
				if (snd_pcm_format_big_endian(format))
					resln.u |=
					    ptr[fmt_phys_width_bytes - 1 -
						k] << k * 8;
				else
					resln.u |= ptr[k] << k * 8;
			}

			/* extend sign of two's complement to fit local storage */
			if (resln.u >= (1U << (fmt_nominal_width_bits - 1))) {
				for (k = fmt_nominal_width_bytes;
				     k < (int)sizeof(resln.u); k++)
					resln.u |= 0xff << k * 8;
			}

			dst[i + (psize * j)] = resln.i;
                        printf("resin is %d", resln.i);
			/* only dumping channel 0 raw pcm in shm for plotting program */
		/*	if (j == 0 && raw_capture_data_map != NULL)
				((int32_t *) raw_capture_data_map)[i] = resln.i;
*/
		}		/* for(j) */
	}			/* for(i) */
}

void *analyze_buffer(void* buff) {
    deinterleave();
    char* buffer = (char*) buff;
    int red_count = 0;
    int blue_count = 0;
    int green_count = 0;
    int buff_size = 7526;
    char* freeze_buf = (char *)malloc(buff_size);
    // red spectrum 1000 - 3000
    // green spectrum 3000 - 5000
    // blue spectrum 5000 - buff_sz
    while (1) {
        // get current buffer before it is modified again by reading/writing
        freeze_buf = memcpy(freeze_buf, buff, buff_size);
        red_count = 0;
        blue_count = 0;
        green_count = 0;
        for (int i = 1000; i < 3000; i++) {
            if (freeze_buf[i] > 0x10) {
                red_count++;
            }
        }        
        for (int i = 3000; i < 5000; i++) {
            if (freeze_buf[i] > 0x10) {
                green_count++;
            }
        }
        for (int i = 5000; i < buff_size; i++) {
            if (freeze_buf[i] > 0x10) {
                blue_count++;
            }
        }

        if (red_count/1000 > 0) {
            setLED(PCA9685_RED_ADDRESS, .1, 0x3ff);
        }
        else {
            setLED(PCA9685_RED_ADDRESS, 0, 0x3ff);
        }

        if (green_count/1000 > 0) {
            setLED(PCA9685_GREEN_ADDRESS, .1, 0x3ff);
        }
        else {
            setLED(PCA9685_GREEN_ADDRESS, 0, 0x3ff);
        }

        if (blue_count/1000 > 0) {
            setLED(PCA9685_BLUE_ADDRESS, .1, 0x3ff);
        }
        else {
            setLED(PCA9685_BLUE_ADDRESS, 0, 0x3ff);
        }
        // sleep for .1 sec
        usleep(100);
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
    
    pthread_t tid;
    pthread_create(&tid, NULL, analyze_buffer, (void *)buffer);

	while (1) {
		snd_pcm_readi(inhandle, buffer, (frames));
		snd_pcm_writei(outhandle, buffer, framesout);
	}
    pthread_join(tid, NULL);
    
	snd_pcm_drain(inhandle);
	snd_pcm_close(inhandle);
	free(buffer);


	snd_pcm_drain(outhandle);
	snd_pcm_close(outhandle);
	free(buff);

	return 0;
}



