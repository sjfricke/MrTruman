#include "main.h"

extern server_t* g_server;

static char command[256];

void webDataCallback( int type, char* value) {
  int val;
  switch(type) {

  // PLACEHOLDER Callback 
  case 0:
    // Returns case 0 of animation is busy
    break;

  // LIGHTS Callback  
  case 1:
    val = atoi(value);
    usleep(100000); // 100ms
    if(val == 1){
    	setLED(PCA9685_RED_ADDRESS, .99, 0x3ff);
	setLED(PCA9685_BLUE_ADDRESS, .5, 0x3ff);
	setLED(PCA9685_GREEN_ADDRESS, .5, 0x3ff);
	lights_on = TRUE;
    }
    else{
	setLED(PCA9685_ALL_CALL, 0, 0x3FF);
	lights_on = FALSE;
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

	// ffmpeg takes a small delay to take picture
	// This is a dirty way of matching timing for flash of camera
	if (fork()==0){
	  // Take picture
	  sprintf(command, "ffmpeg -f video4linux2 -s 160x120 -i /dev/video0 -vf vflip -frames 1 ./website/res/img/camera_image%d.jpg -y -loglevel quiet", photo_index);
	  system(command);

	  broadcastInt("4", photo_index);

	  // clean up photos after first one
	  if (photo_index > 3) {
	    sprintf(command, "rm ./website/res/img/camera_image%d.jpg -f", photo_index - 1);
	    system(command);
	  }
	  kill(getpid(), SIGKILL);
	}
	
	usleep(600000);

	// Alert animation
        broadcastString("4","1");

	usleep(400000);
	
        // Turn on (flash)
        setLED(PCA9685_RED_ADDRESS, .99, 0x3ff);
	setLED(PCA9685_BLUE_ADDRESS, .99, 0x3ff);
	setLED(PCA9685_GREEN_ADDRESS, .99, 0x3ff);

	usleep(150000);
	
	// Turn off flash or back to on state
	if (lights_on == TRUE) {
	  setLED(PCA9685_RED_ADDRESS, .99, 0x3ff);
	  setLED(PCA9685_BLUE_ADDRESS, .5, 0x3ff);
	  setLED(PCA9685_GREEN_ADDRESS, .5, 0x3ff);
	} else {
	  setLED(PCA9685_RED_ADDRESS, 0.0, 0x3ff);
	  setLED(PCA9685_BLUE_ADDRESS, 0.0, 0x3ff);
	  setLED(PCA9685_GREEN_ADDRESS, 0.0, 0x3ff);
	}
	
      } else {
	photo_index++; // cant do in fork or it does not get updated in this process
        animation_on = FALSE;
      }
    
    break;

  // SPEAKER/SERVO Callback  
  case 4:
    val = atoi(value);
    if (val == 0) {
      speaker_animation_ready = TRUE;
    }
    else if (val == 1) {
      servoRetract();
      voiceHardwareSetup();
      animation_on = FALSE;
    } else if (val == 2) {
      servoExtend();
    }
    break;

  // GREEK Callback  
  case 5:
    animation_on = FALSE;
    break;
    
 //unused
  case 6:
    animation_on = FALSE;
    break;

  //unused
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

void* pollTemperature(void* x) {
  double currentTemp;
  while(1) {
    currentTemp = getTemp();
    broadcastInt("6", (int)currentTemp);
    usleep(2000000); // 2 sec
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
  speaker_animation_ready = FALSE;
  lights_on = FALSE;
  photo_index = 2; // 0 and 1 are reserved

  // Kick off temperature thread
  int rc = pthread_create(&tempThread, NULL, pollTemperature, NULL);
  if (rc) {
    printf("ERROR: Can't create temperature thread");
  }

  if (fork()==0) {
      sprintf(command, "xinit /usr/bin/chromium http://localhost:6419 --no-sandbox --kiosk");
      system(command);
  } else {
    while(1) {

      // Sleep a millisecond cuz come on, who is it really hurting?
      if(animation_on){ usleep(1000); continue;}

      // We check for the audio plug and gyro in voiceCommand and immediately return from it upon
      // either event, so that voiceCommand(); won't end up blocking.
      voiceCommand();    

      if (audio_plugged_in) {
	sprintf(command, "amixer -c 0 cset iface=MIXER,name='ADC2 MUX' 'INP2'");
	system(command);
	sprintf(command, "amixer cset iface=MIXER,name='ADC2 Volume' 3");
	system(command);
	animation_on = TRUE;
	speaker_animation_ready = FALSE;

	broadcastString("5", "0");

	while(!speaker_animation_ready);
      

	broadcastString("5", "1");
	printf("\nSPEAKER IS READY IN ANIMAION\n");  
	loopbackSetup();
	loopback();
	loopbackTerminate();
	// Turn off lights or back to on state
	if (lights_on == TRUE) {
	  setLED(PCA9685_RED_ADDRESS, .99, 0x3ff);
	  setLED(PCA9685_BLUE_ADDRESS, .5, 0x3ff);
	  setLED(PCA9685_GREEN_ADDRESS, .5, 0x3ff);
	} else {
	  setLED(PCA9685_RED_ADDRESS, 0.0, 0x3ff);
	  setLED(PCA9685_BLUE_ADDRESS, 0.0, 0x3ff);
	  setLED(PCA9685_GREEN_ADDRESS, 0.0, 0x3ff);
	}

      }

      if (gyro_tripped) {
	// blocks until gyro is set back to no tilt for 3 secs straight
	int dir;
	animation_on = TRUE;

	while (gyro_tripped == TRUE) {
	  // time to wait between tilt detect
	  // this will also prevent any noice from having animtion go, will have 3 sec delay though
	  usleep(50000); // 50ms

	  dir = getTiltDirection(); // 1 for right, -1 for left
	  if (dir == 1) {
	    broadcastString("7", "-0.707");
	  } else if (dir == -1) {
	    broadcastString("7", "0.707");
	  } else {
	    usleep(1000000); // 1sec of 3
	    if (0 != getTiltDirection()) { continue; }

	    usleep(1000000); // 2sec of 3
	    if (0 != getTiltDirection()) { continue; }

	    usleep(1000000); // 3sec of 3
	    if (0 != getTiltDirection()) { continue; }

	    // Truman to get back to his feet
	    gyro_tripped = FALSE;
	    gyroClearInterrupt();
	    broadcastString("7", "0");
	  }
	}
      } 

      usleep(10000); // 10ms
    
    }
  }
  // hardwareTests();
  printf("main\n");
}
