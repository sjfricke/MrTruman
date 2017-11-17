#include "main.h"

extern server_t* g_server;

static int status;
static int busy;

void webDataCallback( int type, char* value) {
  switch(type) {

  // PLACEHOLDER Callback 
  case 0:
    break;

  // LIGHTS Callback  
  case 1:
    setLED(PCA9685_RED_ADDRESS, .99, 0x3ff);
		setLED(PCA9685_BLUE_ADDRESS, .5, 0x3ff);
		setLED(PCA9685_GREEN_ADDRESS, .5, 0x3ff);
    animation_on = FALSE;
    break;

  // FAN Callback    
  case 2:
    break;

  // CAMERA Callback
  case 3:
      sprintf(command, "ffmpeg -f video4linux2 -s 640x480 -i /dev/video0 -ss 0:0:2 -frames 1 ./trumanpicture.jpg");
      system(command);
      animation_on = FALSE;
    break;

  // SPEAKER/SERVO Callback  
  case 4:
    int val = atoi(value);
    if(val == 0){
      printf("\nSPEAKER IS READY IN ANIMAION\n");  
      loopback();
      broadcastString("5", "3");
      
    }
    if(val == 1){
      servoExtend();
      sprintf(command, "amixer -c 0 cset iface=MIXER,name='ADC2 MUX' 'INP3'");
      system(command);
      animation_on = FALSE;
    }
    break;


  // GREEK Callback  
  case 5:
    gyroClearInterrupt();
    animation_on = FALSE;
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

  initAuxGPIO();
  loopbackSetup();

  // Calls LSM6DS3h_start and Gryo GPIO
  enableGyroTilt();
  usleep(1000); // Just waiting
  gyroClearInterrupt();

  // Calls PCA9685_Start and Servo GPIO
  //  servoStart();
  initLEDs();
}

static uint8_t headphone_status;

int main ( int argc, char* argv[] ) {

  uint16_t headphone_jack;
  uint16_t gyro_interrupt;
  char command[256];

  g_server = (server_t*)malloc(sizeof(server_t));
  g_server->port = 6419;
  g_server->onData = webDataCallback;

  startServer();
  HardwareSetup();

  headphone_jack = GpioDB410cMapping(23);
  GpioEnablePin(headphone_jack);
  GpioSetDirection(headphone_jack, INPUT_PIN);
  headphone_status = GpioGetValue(headphone_jack);

  uint8_t animation_on = FALSE;
  uint8_t audio_plugged_in = FALSE;
  uint8_t gyro_tripped = FALSE;
  
  while(1) {

    if(animation_on){continue;}
      
    voiceCommand();    

    if (audio_plugged_in) {
      broadcastString("5", "0");
      sprintf(command, "amixer -c 0 cset iface=MIXER,name='ADC2 MUX' 'INP2'");
      system(command);
      servoExtend();
      animation_on = TRUE;
    }

    if (gyro_tripped) {
      broadcastString("7", "0");
      gyroClearInterrupt();
      animation_on = TRUE;
      gyro_tripped = FALSE;
    } 

    usleep(50000); // 50ms
    
  }

  // hardwareTests();
  printf("main\n");
}
