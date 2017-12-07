#include "commands.h"

static char* c_lights = "LIGHT";
static char* c_fire = "FIRE";
static char* c_picture = "PICTURE";
static char* c_selfie = "SELFIE";
static char* c_photo = "PHOTO";
static char* c_chitchat = "WHAT";
static char* c_jump = "JUMP";

static char command[256];
/*
char* _green = "green";
char* _blue = "blue";
char* _yellow = "yellow";
char* testType = "0";
char* testValue = "0";
*/

int commandDetect(char const* voice_str)
{  	
  char* detect;

  printf("DEBUG - Said: %s\n", voice_str);

  // to prevent unwanted playing of commands
  // "Truman" is key word to activate anything
  if(strstr(voice_str, "TRUMAN") == NULL){
    return 0;
  }

  /* 
    Light switch animation
    0 - Turn on
    1 - Turn off
    2 - Toggle Lights 
  */ 
  if ((strstr(voice_str, c_lights) != NULL)) {

    // Play the notification sounds
      if (fork()==0) {
	    // Take picture
	    sprintf(command, "aplay -D plughw:0,1 ./audio/sounds/truman_notification.wav");
	    system(command);
	    kill(getpid(), SIGKILL);
    	}

    char* detectOn = strstr(voice_str, "ON");
    char* detectOff = strstr(voice_str, "OFF");
 
    if((detectOn != NULL) || (detectOff != NULL)){
      (detectOn != NULL) ? broadcastString("1", "0") : broadcastString("1", "1");
      animation_on = TRUE;
    }

    return 0;
  }

  /*
    Fireplace Animations
    0 - Turn on
    1 - Turn off
    2 - Toggle Fire
  */
  else if (strstr(voice_str, c_fire) != NULL) {

    // Play the notification sounds
      if (fork()==0) {
	    // Take picture
	    sprintf(command, "aplay -D plughw:0,1 ./audio/sounds/truman_notification.wav");
	    system(command);
	    kill(getpid(), SIGKILL);
    	}

    char* detectOn = strstr(voice_str, "ON");
    char* detectOff = strstr(voice_str, "OFF");

    if((detectOn != NULL) || (detectOff != NULL)){
    	(detectOn != NULL) ? broadcastString("3", "0") : broadcastString("3", "1");
      animation_on = TRUE;
    }
    
    return 0;
  }

  /*
    Taking pictures animation
  */
  else if ((strstr(voice_str, c_picture) != NULL) || (strstr(voice_str, c_photo) != NULL)) {

      // Play the notification sounds
      if (fork()==0) {
	    // Take picture
	    sprintf(command, "aplay -D plughw:0,1 ./audio/sounds/truman_notification.wav");
	    system(command);
	    kill(getpid(), SIGKILL);
    	}

    broadcastString("4", "0");
    animation_on = TRUE;
    return 0;
  }

  // Chat bubble
  else if ((strstr(voice_str, "WHAT") != NULL) && (strstr(voice_str, "UP") != NULL)) {
    broadcastString("8", "Hello! My name<br>is <b>Mr. Truman!</b>");
    sprintf(command, "aplay -D plughw:0,1 ./audio/sounds/truman_chat.wav");
    system(command);
    return 0;
  }

  else if((strstr(voice_str, "FIDGET") != NULL) && (strstr(voice_str, "SPINNER") != NULL)){
    broadcastString("9", "0");
    return 0;
  }

  else if((strstr(voice_str, "DO") != NULL) && (strstr(voice_str, "MATH") != NULL)){
    sprintf(command, "aplay -D plughw:0,1 ./audio/sounds/truman_quick_maths.wav");
    system(command);
    return 0;
  }

  else if(strstr(voice_str, "RAP") != NULL){
    sprintf(command, "aplay -D plughw:0,1 ./audio/sounds/truman_skraa.wav");
    system(command);
    return 0;
  }

  printf("No command listed found\n");
  broadcastString("8", "???");
  sprintf(command, "aplay -D plughw:0,1 ./audio/sounds/truman_misunderstand.wav");
  system(command);
  return -1;
}
