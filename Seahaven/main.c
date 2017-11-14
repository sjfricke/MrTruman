#include "main.h"

extern server_t* g_server;

static int status;
static int busy;

void webDataCallback( int type, char* value) {
  switch(type) {
  case 0:
    break;
  case 1:
    break;
  case 2:
    break;
  case 3:
    break;
  case 4:
    break;
  case 5:
    break;
  default:
    printf("Not a valid type! [%d]\n", type);
    break;
  }
}

void HardwareSetup() {
  // audio voice
  voiceHardwareSetup();
  voiceDictionarySetup();

  // Calls LSM6DS3h_start and Gryo GPIO
  //  enableGyroTilt();

  // Calls PCA9685_Start and Servo GPIO
  //  servoStart();

  // initLEDs();
}

int main ( int argc, char* argv[] ) {
/*
  uint16_t headphone_jack;

  g_server = (server_t*)malloc(sizeof(server_t));
  g_server->port = 6419;
  g_server->onData = webDataCallback;

  startServer();

  HardwareSetup();

  headphone_jack = GpioDB410cMapping(23);
  GpioEnablePin(headphone_jack);
  GpioSetDirection(headphone_jack, INPUT_PIN);

  
  while(1) {
    printf("START TALKING\n");
    voiceCommand();
    usleep(50000); // 50ms
    
  }
*/
  FILE * audiofile = fopen("./audiosamples.txt", "r");
  FILE * processed = fopen("PROCSAMPLES.txt", "w");

  int tmp = 0;
  int16_t sample=0;
  int go = 1;
  int count = 0;
  while(count < 500000){
	  
    sample = fgetc(audiofile);
//    if(sample < EOF) go = 0;
    sample |= fgetc(audiofile) << 8;
    fprintf(processed,"%d\n",sample);
    count++;
  }
  fclose(audiofile);
  fclose(processed);

  // hardwareTests();
  printf("main\n");
}
