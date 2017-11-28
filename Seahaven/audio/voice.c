#include "voice.h"
#include "../status.h"

// Single instance of PocketSphinx voice
// ps variables
static ps_decoder_t *ps;
static cmd_ln_t *config;

// mic in variables
static ad_rec_t *ad;
static int16_t adbuf[PS_BUF_SIZE];
static uint32_t buf_count;
static int32_t k;

char const* voice_str;

extern uint8_t audio_plugged_in;
extern uint8_t animation_on;
extern uint8_t gyro_tripped; 


void voiceHardwareSetup() {
  char command[256];

  // Set up output through speakers
  sprintf(command, "amixer -c 0 cset iface=MIXER,name='SPK DAC Switch' 1");
  system(command);
  sprintf(command, "amixer -c 0 cset iface=MIXER,name='RX3 MIX1 INP1' 'RX1'");
  system(command);
  sprintf(command, "amixer cset iface=MIXER,name='RX3 Digital Volume' 136");
  system(command);

  // default input to mic over audio jack
  sprintf(command, "amixer -c 0 cset iface=MIXER,name='DEC1 MUX' 'ADC2'");
  system(command);
  sprintf(command, "amixer cset iface=MIXER,name='ADC2 Volume' 2");
  system(command);
  // INP3 is for mic on board, INP2 is for auxilliary line in.
  sprintf(command, "amixer -c 0 cset iface=MIXER,name='ADC2 MUX' 'INP3'");
  system(command);
}

void voiceDictionarySetup() {
  config = cmd_ln_init(NULL, ps_args(), TRUE,
		       "-hmm", "/usr/local/share/pocketsphinx/model/en-us/en-us",
		       "-lm", PS_TRUMAN_LM,
		       "-dict", PS_TRUMAN_DICT,
		       NULL);
  
  if (config == NULL) {
    fprintf(stderr, "Failed to create config object, see log for details\n");
    exit(-1);
  }

  ps = ps_init(config);
  if (ps == NULL) {
    fprintf(stderr, "Failed to create recognizer, see log for details\n");
    exit(-1);
  }
}

int voiceCommand() {

  if ((ad = ad_open_dev(PS_ALSA_VOICE_MIC_HW, 16000)) == NULL) {
    fprintf(stderr, "Failed to open audio device\n");
    exit(-1);
  }

  // start recording
  if (ad_start_rec(ad) < 0) {
    fprintf(stderr, "Failed to start recording\n");
    return -1;
  }

  if (ps_start_utt(ps) < 0) {
    fprintf(stderr, "Failed to start utterance\n");
    return -1;
  }

  uint8_t utt_started = 0;
  uint8_t in_speech;
  uint16_t headphone_jack;
  uint16_t gyro_int;
  headphone_jack = GpioDB410cMapping(23);

  //  buf_count = 15;
  for (;;) {

    if(GpioGetValue(headphone_jack) == 1){ 
	printf("STARTED\n");
      ps_end_utt(ps);
	ad_close(ad);
	//voiceCleanUp();
	printf("MADE IT\n");
	    audio_plugged_in = TRUE; 
	    return 0; 
    }
    if(gyroInterruptPoll()) {

      ps_end_utt(ps);
	ad_close(ad);
	//voiceCleanUp();
	    gyro_tripped = TRUE; 
	    return 0; 
    }

    // Start getting data
    if ((k = ad_read(ad, adbuf, PS_BUF_SIZE)) < 0) {
      fprintf(stderr, "Failed to read audio\n");
      ad_close(ad);
      return -1;
    }
    
    //printf("process...\n");
    ps_process_raw(ps, adbuf, k, FALSE, FALSE);
    in_speech = ps_get_in_speech(ps);
    //buf_count--;
    //    printf("Count: %d\n", buf_count);

    if (in_speech && !utt_started) {
      utt_started = 1;
    }
    
    if (!in_speech && utt_started) {
      ps_end_utt(ps);
      voice_str = ps_get_hyp(ps, NULL);
      if (voice_str != NULL) {
	// do all the switch case in seperate file
	ad_close(ad);
	return commandDetect(voice_str);
      } else {
	printf("LOG: VoicE_str was null");
	ad_close(ad);
	return -1;
      }

      if (ps_start_utt(ps) < 0) {
	fprintf(stderr, "Failed to start utterance\n");
	ad_close(ad);
	return -1;
      }
    }
  } // for(;;)
}

void voiceCleanUp()
{
  ps_free(ps);
  cmd_ln_free_r(config);
}
