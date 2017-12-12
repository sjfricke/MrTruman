#include "main.h"

extern server_t* g_server;

static char command[256];
uint16_t speech_marker = 0;

void webDataCallback( int type, char* value) {
  int val;
  FILE *fp; // c99 pre declaring
  
  switch(type) {

    // When Ready call
  case 0:
    browser_connected = TRUE;
    break;

    // LIGHTS Callback  
  case 1:
    val = atoi(value);
    usleep(100000); // 100ms
    if(val == 1){

      // Play the lights on sounds
      soundClipPlay(sc_lights_on, scb_lights_on);

      setLED(PCA9685_RED_ADDRESS, .99, 0x3ff);
      setLED(PCA9685_BLUE_ADDRESS, .5, 0x3ff);
      setLED(PCA9685_GREEN_ADDRESS, .5, 0x3ff);
      lights_on = TRUE;
    } else {

      // Play the lights off sounds
      soundClipPlay(sc_lights_off, scb_lights_off);

      setLED(PCA9685_RED_ADDRESS, 0, 0x3ff);
      setLED(PCA9685_BLUE_ADDRESS, 0, 0x3ff);
      setLED(PCA9685_GREEN_ADDRESS, 0, 0x3ff);
      lights_on = FALSE;
    }
    animation_on = FALSE;
    break;

    // FAN Callback    
  case 2:
    val = atoi(value);
    if(val == 0){
      fanOn(0.95);
      animation_on = FALSE;
    }
    else if (val == 1){      
      fanOff();
      animation_on = FALSE;
    } else if (val == 2) {
	
      usleep(500000); // 500ms
      // Play the fire on sound
      soundClipPlay(sc_fire_on, scb_fire_on);

    } else if (val == 3) {
      // Play the fire off sound
      usleep(500000); // 500ms
      soundClipPlay(sc_fire_off, scb_fire_off);
    }
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

      // Play the camera shutter sounds
      soundClipPlay(sc_camera, scb_camera);

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
      voiceHardwareSetup();
      animation_on = FALSE;
    } else if (val == 2) {
      
    }
    break;

    // GREEK Callback  
  case 5:
    val = atoi(value);
    if (val == 0) {
      // back to normal      
      animation_on = FALSE;
    }
    else if (val == 1) {
      // truman hit wall
      soundClipPlay(sc_thud_truman, scb_thud_truman);
    } else if (val == 2) {
      // couch hit wall
      soundClipPlay(sc_thud_couch, scb_thud_couch);
    }
    break;
    
    // when to play speech bubble sounds
  case 6:
    val = atoi(value);
    speech_marker = val+1;
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

  case 9:
    val = atoi(value);

    if (val == 0) {
      setLED(PCA9685_RED_ADDRESS, 0.0, 0x3ff);
	setLED(PCA9685_BLUE_ADDRESS, 0.0, 0x3ff);
	setLED(PCA9685_GREEN_ADDRESS, 0.0, 0x3ff);
      sprintf(command, "halt");
      system(command);
      // Bye Bye
    }
    else if (val == 1) {
      setLED(PCA9685_RED_ADDRESS, 0.0, 0x3ff);
	setLED(PCA9685_BLUE_ADDRESS, 0.0, 0x3ff);
	setLED(PCA9685_GREEN_ADDRESS, 0.0, 0x3ff);
      sprintf(command, "reboot");
      system(command);
      // Bye Bye
    }    
    break;

    // write out highscore
  case 10:
    fp = fopen("website/src/highscore.js", "w");
    if (fp == NULL) {
      printf("ERROR: can't write, leaving to prevent fault\n");
      return;
    }
    fprintf(fp, "var highscore = %s;", value);
    fclose(fp);
    break;
    
  default:
    printf("Not a valid type! [%d]\n", type);
    break;
  }
}

void* pollTemperature(void* x) {
  double currentTemp;
  int fidgetCount = 0;
  while(1) {
    currentTemp = getTemp();
    broadcastInt("6", (int)currentTemp);
    usleep(2000000); // 2 sec

    fidgetCount++;
    if (fidgetCount > 3) {
      fidgetCount = 0;
      broadcastInt("9",0);
    }
  }
}

// Sets up a whole bunch of hardware peripherals 
// ensures that the truman experience will be buttery smooth
void HardwareSetup() {

  // audio voice
  voiceHardwareSetup();
  voiceDictionarySetup();

  initAuxGPIO();
  //loopbackSetup();

  // Load all the clips into memory for 5MB of fast access dankness (V important)
  soundClipSetup();
  
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
  audio_threshold = FALSE;
  browser_connected = FALSE;

  while (browser_connected == FALSE) {
    usleep(1000);
  }

  // Kick off temperature thread
  int rc = pthread_create(&tempThread, NULL, pollTemperature, NULL);
  if (rc) {
    printf("ERROR: Can't create temperature thread");
  }

  while(1) {

    if(speech_marker != 0){
      if (speech_marker == 1) {
        speech_marker = 1;
        soundClipPlay(sc_misunderstand, scb_misunderstand);
        usleep(2000000); // 2 sec
      } else if (speech_marker == 2) {
        soundClipPlay(sc_chat, scb_chat);
        usleep(2000000); // 2 sec
      } else if (speech_marker == 3) {
        soundClipPlay(sc_not_hot, scb_not_hot);
      } else if (speech_marker == 4) {
        soundClipPlay(sc_quick_maths, scb_quick_maths);
      } else if (speech_marker == 5) {
        soundClipPlay(sc_skraa, scb_skraa);
      } else if (speech_marker == 6) {
        soundClipPlay(sc_i_did_not, scb_i_did_not);
      }

      speech_marker = 0;
      broadcastString("2", "0"); // ends animation bubble
      animation_on = FALSE;
    
    }
    
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
      broadcastString("5", "2");
      while(!speaker_animation_ready);

      servoExtend();
      
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

      servoRetract();
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
