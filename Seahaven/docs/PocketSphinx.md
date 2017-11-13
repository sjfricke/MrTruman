# How to install PocketSphinx on Dragonboard Linaro

> Tested on Linaro 4.9.39

## Update board

- `sudo apt-get update`
- `sudo apt-get upgrade`
- `sudo apt-get install autoconf libtool automake bison python-dev swig libasound2-dev`

## Get PocketSphinx and SphinxBase

- `git clone https://github.com/cmusphinx/pocketsphinx.git`
- `git clone https://github.com/cmusphinx/sphinxbase.git`

## Setting up

- `cd sphinxbase`
- `./autogen.sh`
- `./configure`
- `make`
- `sudo make install`
- `cd ../pocketsphinx`
- `./autogen.sh`
- `./configure`
- `make`
- `sudo make install`

## Turn off PulseAudio

We need to make sure PulseAudio is off because we are using ALSA

`apt purge libpulse-dev`


## Run

- `export LD_LIBRARY_PATH=/usr/local/lib`
- `export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig`

- `pocketsphinx_continuous -adcdev plughw:0,2 -infile <wav.file>`
- `pocketsphinx_continuous -adcdev plughw:0,2 -inmic yes`
    - Note with ALSA it will complain about buffer size being overwritten too fast, this is not an issue when running a custom program
   
   Follow this link to upload a custom recognition file - [http://www.speech.cs.cmu.edu/tools/lmtool-new.html]
- `pocketsphinx_continuous -adcdev plughw:0,2 -inmic yes -dict <custom_dictionary.dic> -lm <custom_language_model.lm>`
  

## Hello World

```
#include <pocketsphinx.h>

int
main(int argc, char *argv[])
{
    ps_decoder_t *ps;
    cmd_ln_t *config;
    FILE *fh;
    char const *hyp, *uttid;
    int16 buf[512];
    int rv;
    int32 score;

    config = cmd_ln_init(NULL, ps_args(), TRUE,
		         "-hmm", MODELDIR "/en-us/en-us",
		         "-lm", MODELDIR "/en-us/en-us.lm.bin",
	    		 "-dict", MODELDIR "/en-us/cmudict-en-us.dict",
		         NULL);
    if (config == NULL) {
	fprintf(stderr, "Failed to create config object, see log for details\n");
	return -1;
    }

    ps = ps_init(config);
    if (ps == NULL) {
	fprintf(stderr, "Failed to create recognizer, see log for details\n");
	return -1;
    }

    fh = fopen("goforward.raw", "rb");
    if (fh == NULL) {
	fprintf(stderr, "Unable to open input file goforward.raw\n");
	return -1;
    }

    rv = ps_start_utt(ps);

    while (!feof(fh)) {
	size_t nsamp;
	nsamp = fread(buf, 2, 512, fh);
	rv = ps_process_raw(ps, buf, nsamp, FALSE, FALSE);
    }

    rv = ps_end_utt(ps);
    hyp = ps_get_hyp(ps, &score);
    printf("Recognized: %s\n", hyp);

    fclose(fh);
    ps_free(ps);
    cmd_ln_free_r(config);

    return 0;
}
```
