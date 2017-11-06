#include "voice.h"

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

void voiceDictionarySetup()
{

  config = cmd_ln_init(NULL, ps_args(), TRUE,
		       "-hmm", "/usr/local/share/pocketsphinx/model/en-us/en-us",
		       "-lm", "/usr/local/share/pocketsphinx/model/en-us/en-us.lm.bin",
		       "-dict", "/usr/local/share/pocketsphinx/model/en-us/cmudict-en-us.dict",
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

int voiceCommand()
{
  if ((ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"), 16000)) == NULL) {
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

  buf_count = 15;
  for (;;) {

    // Start getting data
    if ((k = ad_read(ad, adbuf, PS_BUF_SIZE)) < 0) {
      fprintf(stderr, "Failed to read audio\n");
      ad_close(ad);
      return -1;
    }

    printf("process...\n");
    ps_process_raw(ps, adbuf, k, FALSE, FALSE);
    //    in_speech = ps_get_in_speech(ps);
    buf_count--;
    printf("Count: %d\n", buf_count);
    
    if (buf_count <= 0) {
      ps_end_utt(ps);
      voice_str = ps_get_hyp(ps, NULL);
      if (voice_str != NULL) {
	printf("Said: %s\n", voice_str);
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
