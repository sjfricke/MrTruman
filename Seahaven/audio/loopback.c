/* This code performs the line-in audio loopback functionality of the Truman_board */
#include "loopback.h"

snd_pcm_t *inhandle;
snd_pcm_t *outhandle;
snd_pcm_uframes_t frames, framesout;
void *buffer;

double PI;
typedef double complex cplx;

void loopbackSetup() {
    initLEDs();
    loopInputSetup();
    loopOutputSetup();
}

void loopbackTerminate() {
	snd_pcm_drain(inhandle);
	snd_pcm_close(inhandle);
	snd_pcm_drain(outhandle);
	snd_pcm_close(outhandle);
	free(buffer);
}

void *analyze_buffer(void* buff) {
	int16_t* buffer = (int16_t*) buff;

	while (1) {
		// get current buffer before it is modified again by reading/writing
		int16_t sample = buffer[0];
		
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
	}
}

static void loopOutputSetup() {

	//////////////////////////////////////////////////
	///////////////// OUTPUT SETUP ///////////////////
	//////////////////////////////////////////////////

	unsigned int pcm, tmp;
	int rate, channels, seconds, buff_size;
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
	buffer = (void *) malloc(buff_size);

	/* We want to loop for 5 seconds */
	snd_pcm_hw_params_get_period_time(params,
			&val, &dir);
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
}

int loopback() {
    uint16_t pin = GpioDB410cMapping(23);
    pthread_t tid;
    int wr;


	/* Audio data dump */
	FILE *datfile = fopen("./audiosamples.txt","w");

	pthread_t tid;
	pthread_create(&tid, NULL, analyze_buffer, (void *)buffer);
    // loop the entire time the aux cord is plugged in
	while (GpioGetValue(pin) == 0) {
		snd_pcm_readi(inhandle, buffer, frames);	
	    wr = snd_pcm_writei(outhandle, buffer, framesout);	
		if (wr < 0) {
		  printf("WRITE ERR %s\n", snd_strerror(wr));
		  snd_pcm_recover(outhandle, wr, 0);
		}
	}

	/* Audio data dump close */
	fclose(datfile);

	pthread_join(tid, NULL);

	return 0;
}



////////////////////// SAMPLE FFT CODE 1 ////////////////////

/* INPUT: float input[16], float output[16] */
/* OUTPUT: none */
/* EFFECTS:  Places the 16 point fft of input in output in a strange */
/* order using 10 real multiplies and 79 real adds. */
/* Re{F[0]}= out0 */
/* Im{F[0]}= 0 */
/* Re{F[1]}= out8 */
/* Im{F[1]}= out12 */
/* Re{F[2]}= out4 */
/* Im{F[2]}= -out6 */
/* Re{F[3]}= out11 */
/* Im{F[3]}= -out15 */
/* Re{F[4]}= out2 */
/* Im{F[4]}= -out3 */
/* Re{F[5]}= out10 */
/* Im{F[5]}= out14 */
/* Re{F[6]}= out5 */
/* Im{F[6]}= -out7 */
/* Re{F[7]}= out9 */
/* Im{F[7]}= -out13 */
/* Re{F[8]}= out1 */
/* Im{F[8]}=0 */
/* F[9] through F[15] can be found by using the formula */
/* Re{F[n]}=Re{F[(16-n)mod16]} and Im{F[n]}= -Im{F[(16-n)mod16]} */

/* Note using temporary variables to store intermediate computations */
/* in the butterflies might speed things up.  When the current version */
/* needs to compute a=a+b, and b=a-b, I do a=a+b followed by b=a-b-b.  */
/* So practically everything is done in place, but the number of adds */
/* can be reduced by doinc c=a+b followed by b=a-b. */

/* The algorithm behind this program is to find F[2k] and F[4k+1] */
/* seperately.  To find F[2k] we take the 8 point Real FFT of x[n]+x[n+8] */
/* for n from 0 to 7.  To find F[4k+1] we take the 4 point Complex FFT of */
/* exp(-2*pi*j*n/16)*{x[n] - x[n+8] + j(x[n+12]-x[n+4])} for n from 0 to 3.*/

void sixteenFFT(float input[16],float output[16]) {
  float temp, out0, out1, out2, out3, out4, out5, out6, out7, out8;
  float out9,out10,out11,out12,out13,out14,out15;

  out0=input[0]+input[8]; /* output[0 through 7] is the data that we */
  out1=input[1]+input[9]; /* take the 8 point real FFT of. */
  out2=input[2]+input[10];
  out3=input[3]+input[11];
  out4=input[4]+input[12];
  out5=input[5]+input[13];
  out6=input[6]+input[14];
  out7=input[7]+input[15];

  out8=input[0]-input[8];   /* inputs 8,9,10,11 are */
  out9=input[1]-input[9];   /* the Real part of the */
  out10=input[2]-input[10]; /* 4 point Complex FFT inputs.*/
  out11=input[3]-input[11]; 
  out12=input[12]-input[4]; /* outputs 12,13,14,15 are */
  out13=input[13]-input[5]; /* the Imaginary pars of  */
  out14=input[14]-input[6]; /* the 4 point Complex FFT inputs.*/
  out15=input[15]-input[7];

  /*First we do the "twiddle factor" multiplies for the 4 point CFFT */
  /*Note that we use the following handy trick for doing a complex */
  /*multiply:  (e+jf)=(a+jb)*(c+jd) */
  /*   e=(a-b)*d + a*(c-d)   and    f=(a-b)*d + b*(c+d)  */

  /* C_M_S_2PI/16=cos(2pi/16)-sin(2pi/16) when replaced by macroexpansion */
  /* C_P_S_2PI/16=cos(2pi/16)+sin(2pi/16) when replaced by macroexpansion */
  /* (SIN_2PI_16)=sin(2pi/16) when replaced by macroexpansion */
  temp=(out13-out9)*(SIN_2PI_16); 
  out9=out9*(C_P_S_2PI_16)+temp; 
  out13=out13*(C_M_S_2PI_16)+temp;
  
  out14*=(SIN_4PI_16);
  out10*=(SIN_4PI_16);
  out14=out14-out10;
  out10=out14+out10+out10;
  
  temp=(out15-out11)*(SIN_6PI_16);
  out11=out11*(C_P_S_6PI_16)+temp;
  out15=out15*(C_M_S_6PI_16)+temp;

  /* The following are the first set of two point butterfiles */
  /* for the 4 point CFFT */

  out8+=out10;
  out10=out8-out10-out10;

  out12+=out14;
  out14=out12-out14-out14;

  out9+=out11;
  out11=out9-out11-out11;

  out13+=out15;
  out15=out13-out15-out15;

  /*The followin are the final set of two point butterflies */
  output[1]=out8+out9;
  output[7]=out8-out9;

  output[9]=out12+out13;
  output[15]=out13-out12;
  
  output[5]=out10+out15;        /* implicit multiplies by */
  output[13]=out14-out11;        /* a twiddle factor of -j */                            
  output[3]=out10-out15;  /* implicit multiplies by */
  output[11]=-out14-out11;  /* a twiddle factor of -j */

  
  /* What follows is the 8-point FFT of points output[0-7] */
  /* This 8-point FFT is basically a Decimation in Frequency FFT */
  /* where we take advantage of the fact that the initial data is real*/

  /* First set of 2-point butterflies */
    
  out0=out0+out4;
  out4=out0-out4-out4;
  out1=out1+out5;
  out5=out1-out5-out5;
  out2+=out6;
  out6=out2-out6-out6;
  out3+=out7;
  out7=out3-out7-out7;

  /* Computations to find X[0], X[4], X[6] */
  
  output[0]=out0+out2;
  output[4]=out0-out2;
  out1+=out3;
  output[12]=out3+out3-out1;

  output[0]+=out1;  /* Real Part of X[0] */
  output[8]=output[0]-out1-out1; /*Real Part of X[4] */
  /* out2 = Real Part of X[6] */
  /* out3 = Imag Part of X[6] */
  
  /* Computations to find X[5], X[7] */

  out5*=SIN_4PI_16;
  out7*=SIN_4PI_16;
  out5=out5-out7;
  out7=out5+out7+out7;

  output[14]=out6-out7; /* Imag Part of X[5] */
  output[2]=out5+out4; /* Real Part of X[7] */
  output[6]=out4-out5; /*Real Part of X[5] */
  output[10]=-out7-out6; /* Imag Part of X[7] */

}

void fftTest() {

  FILE* procsamps = fopen("./audio/sampledataproc.txt", "r");	
  float data[16];
  float output[16];
  float zero=0;

  printf("\ntype 16 point input vector\n");
  scanf("%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",&data[0],&data[1],&data[2],&data[3],&data[4],&data[5],&data[6],&data[7],&data[8],&data[9],&data[10],&data[11],&data[12],&data[13],&data[14],&data[15]);
  
  sixteenFFT(data,output);
  printf("\nresult is:\n");
  printf("k,\t\tReal Part\t\tImaginary Part\n");
    printf("0\t\t%.9f\t\t%.9f\n",output[0],zero);
  printf("1\t\t%.9f\t\t%.9f\n",output[1],output[9]);
  printf("2\t\t%.9f\t\t%.9f\n",output[2],output[10]);
  printf("3\t\t%.9f\t\t%.9f\n",output[3],output[11]);
  printf("4\t\t%.9f\t\t%.9f\n",output[4],output[12]);
  printf("5\t\t%.9f\t\t%.9f\n",output[5],output[13]);
  printf("6\t\t%.9f\t\t%.9f\n",output[6],output[14]);
  printf("7\t\t%.9f\t\t%.9f\n",output[7],output[15]);
  printf("8\t\t%.9f\t\t%.9f\n",output[8],zero);
  printf("9\t\t%.9f\t\t%.9f\n",output[7],-output[15]);
  printf("10\t\t%.9f\t\t%.9f\n",output[6],-output[14]);
  printf("11\t\t%.9f\t\t%.9f\n",output[5],-output[13]);
  printf("12\t\t%.9f\t\t%.9f\n",output[4],-output[12]);
  printf("13\t\t%.9f\t\t%.9f\n",output[3],-output[11]);
  printf("14\t\t%.9f\t\t%.9f\n",output[2],-output[9]);
  printf("15\t\t%.9f\t\t%.9f\n",output[1],-output[8]);
}


/////////// SAMPLE FFT CODE 2 ///////////////////
 
void _fft(cplx buf[], cplx out[], int n, int step)
{
	if (step < n) {
		_fft(out, buf, n, step * 2);
		_fft(out + step, buf + step, n, step * 2);
 
		for (int i = 0; i < n; i += 2 * step) {
			cplx t = cexp(-I * PI * i / n) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}
 
void fft(cplx buf[], int n)
{
	cplx out[n];
	for (int i = 0; i < n; i++) out[i] = buf[i];
 
	_fft(buf, out, n, 1);
}
 
 
void show(const char * s, cplx buf[]) 
{
	printf("%s", s);
	for (int i = 0; i < 8; i++)
		if (!cimag(buf[i]))
			printf("%g ", creal(buf[i]));
		else
			printf("(%g, %g) ", creal(buf[i]), cimag(buf[i]));
}


int fftTest2()
{
	PI = atan2(1, 1) * 4;
	cplx buf[] = {1, 1, 1, 1, 0, 0, 0, 0};
 
	show("Data: ", buf);
	fft(buf, 8);
	show("\nFFT : ", buf);
 
	return 0;
}










