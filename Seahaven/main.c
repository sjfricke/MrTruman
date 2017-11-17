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
    printf("\nSPEAKER IS READY IN ANIMAION\n");
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

static uint8_t headphone_status;

int main ( int argc, char* argv[] ) {

  uint16_t headphone_jack;
  
  g_server = (server_t*)malloc(sizeof(server_t));
  g_server->port = 6419;
  g_server->onData = webDataCallback;

  startServer();

  HardwareSetup();

  headphone_jack = GpioDB410cMapping(23);
  GpioEnablePin(headphone_jack);
  GpioSetDirection(headphone_jack, INPUT_PIN);
  headphone_status = GpioGetValue(headphone_jack);
  
  
  while(1) {

    if (GpioGetValue(headphone_jack) == 1) {
      broadcastString("5", "0");
      headphone_status = 1;
    } else if (headphone_status == 1 && GpioGetValue(headphone_jack) == 0) {
      // unplugged
      broadcastString("5", "3");
      headphone_status = 0;
    }
    
    // printf("START TALKING\n");    
    //    voiceCommand();    
    usleep(50000); // 50ms
    
  }

  // hardwareTests();
  printf("main\n");
}
