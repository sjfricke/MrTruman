#include "main.h"

extern server_t* g_server;

extern uint8_t audio_plugged_in;
extern uint8_t animation_on;
extern uint8_t gyro_tripped; 

static char command[256];


void webDataCallback( int type, char* value) {
  int val;
  switch(type) {

  // PLACEHOLDER Callback 
  case 0:
    // NOTHING YO
    break;

  // LIGHTS Callback  
  case 1:
    val = atoi(value);
    if(val == 1){
    	setLED(PCA9685_RED_ADDRESS, .99, 0x3ff);
	setLED(PCA9685_BLUE_ADDRESS, .5, 0x3ff);
	setLED(PCA9685_GREEN_ADDRESS, .5, 0x3ff);
    }
    else{
	setLED(PCA9685_ALL_CALL, 0, 0x3FF);
    }
    animation_on = FALSE;
    break;

  // FAN Callback    
  case 2:
    val = atoi(value);
    if(val == 0){
      fanOn(0.95);
    }
    else {
      fanOff();
    }
    animation_on = FALSE;

    break;

  // CAMERA Callback
  case 3:
      val = atoi(value);
      if(val == 0){
        // Alert animation
        broadcastString("4","1");
        // Turn on (flash)
        setLED(PCA9685_RED_ADDRESS, .99, 0x3ff);
	    	setLED(PCA9685_BLUE_ADDRESS, .5, 0x3ff);
	    	setLED(PCA9685_GREEN_ADDRESS, .5, 0x3ff);
        // Take picture
        sprintf(command, "ffmpeg -f video4linux2 -s 640x480 -i /dev/video0 -ss 0:0:2 -frames 1 ./trumanpicture.jpg");
        system(command);
        // Turn off
        setLED(PCA9685_RED_ADDRESS, 0, 0x3ff);
	    	setLED(PCA9685_BLUE_ADDRESS, 0, 0x3ff);
	    	setLED(PCA9685_GREEN_ADDRESS, 0, 0x3ff);
      } else {
        broadcastString("4","2");
        animation_on = FALSE;
      }
    
    break;

  // SPEAKER/SERVO Callback  
  case 4:
    val = atoi(value);
    if(val == 0){
      broadcastString("5", "1");
      //printf("\nSPEAKER IS READY IN ANIMAION\n");  
      loopbackSetup();
      loopback();
      loopbackTerminate();
    }
    if(val == 1){
      //servoExtend();
      /*sprintf(command, "amixer -c 0 cset iface=MIXER,name='ADC2 MUX' 'INP3'");
      printf("in case 4\n");
      system(command);*/
	animation_on = FALSE;
      voiceHardwareSetup();
      printf("SETTING ANIMATION_ON FALSE\n");
      animation_on = FALSE;
    }
    break;

  // GREEK Callback  
  case 5:
    gyroClearInterrupt();
    // Need to really check the direction and send it back. TODO
    animation_on = FALSE;
    break;
    
  // Chat interaction over.
  case 6:
    animation_on = FALSE;
    break;

  // Jumping completed.
  case 7:
    animation_on = FALSE;
    break;

  case 8:
    val = atoi(value);
    val = val/2.2 + 100;
    printf("Case 8:\n\tgot value %d for volume\n", val);
    sprintf(command, "amixer cset iface=MIXER,name='RX3 Digital Volume' %d", val);
    system(command);
    animation_on = FALSE;
    break;

  default:
    printf("Not a valid type! [%d]\n", type);
    break;
  }
}

// Sets up a whole bunch of hardware peripherals 
// ensures that the truman experience will be buttery smooth
void HardwareSetup() {

  // audio voice
  voiceHardwareSetup();
  voiceDictionarySetup();

  initAuxGPIO();
 // loopbackSetup();

  // Calls LSM6DS3h_start and Gryo GPIO
  enableGyroTilt();
  usleep(1000); // Just waiting
  gyroClearInterrupt();

  // Calls PCA9685_Start and Servo GPIO
  servofanStart();
  initLEDs();
  fanOff();
  servoStop();
  setLED(PCA9685_ALL_CALL, 0, 0x3FF);
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

  // Set global variables
  animation_on = FALSE;
  audio_plugged_in = FALSE;
  gyro_tripped = FALSE;
  

  while(1) {


    if(animation_on){ usleep(1000); continue;} // Sleep a millisecond cuz come on, who is it really hurting?

    // We check for the audio plug and gyro in voiceCommand and immediately return from it upon
    // either event, so that voiceCommand(); won't end up blocking.
    voiceCommand();    

    if (audio_plugged_in) {
	   printf("AUDIOBLOCK\n");
      sprintf(command, "amixer -c 0 cset iface=MIXER,name='ADC2 MUX' 'INP2'");
      system(command);
       sprintf(command, "amixer cset iface=MIXER,name='ADC2 Volume' 3");
      system(command);

      broadcastString("5", "0");
      //servoExtend();
      servoStop();
      animation_on = TRUE;
    }

    if (gyro_tripped) {
	    printf("GYROBLOCK\n");
      int dir = getTiltDirection();
      if(dir == 1 || dir == -1){
        // 1 for right, -1 for left
        sprintf(command, "%d", dir);
        broadcastString("7", command);
        gyroClearInterrupt();
        animation_on = TRUE;
        gyro_tripped = FALSE;
      } else{
        animation_on = FALSE;
        gyro_tripped = FALSE;
        gyroClearInterrupt();
      }
      
    } 

    usleep(50000); // 50ms
    
  }

  // hardwareTests();
  printf("main\n");
}
