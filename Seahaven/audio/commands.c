#include "commands.h"

static char* c_lights = "LIGHT";
static char* c_fire = "FIRE";
static char* c_picture = "PICTURE";
static char* c_selfie = "SELFIE";
static char* c_photo = "PHOTO";
static char* c_chitchat = "WHAT";
static char* c_jump = "JUMP";

static char command[256];

int commandDetect(char const* voice_str)
{  	
  char* detect;

  printf("DEBUG - Said: %s\n", voice_str);

  // Got help?
  if(strstr(voice_str, "HELP") != NULL){
    soundClipPlay(sc_notification, scb_notification);
    broadcastString("11", "0");
  }
  // to prevent unwanted playing of commands
  // "Truman" is key word to activate anything
  if(strstr(voice_str, "TRUMAN") == NULL){
    return 0;
  }

  char* detectOn = strstr(voice_str, "ON");
  char* detectOff = strstr(voice_str, "OFF");

  /* 
    Light switch animation
    0 - Turn on
    1 - Turn off
    2 - Toggle Lights 
  */ 
  if ((strstr(voice_str, c_lights) != NULL)) {

    if((detectOn != NULL) || (detectOff != NULL)){
      // Play the notification sounds
      soundClipPlay(sc_notification, scb_notification);
      (detectOn != NULL) ? broadcastString("1", "0") : broadcastString("1", "1");
	    usleep(1000);
      animation_on = TRUE;
      return 0;
    }

  }

  /*
    Fireplace Animations
    0 - Turn on
    1 - Turn off
    2 - Toggle Fire
  */
  else if (strstr(voice_str, c_fire) != NULL) {

    if((detectOn != NULL) || (detectOff != NULL)){
      // Play the notification sounds
      soundClipPlay(sc_notification, scb_notification);
    	(detectOn != NULL) ? broadcastString("3", "0") : broadcastString("3", "1");
      animation_on = TRUE;
      return 0;
    }

  }

  /*
    Taking pictures animation
  */
  else if ((strstr(voice_str, c_picture) != NULL) || (strstr(voice_str, c_photo) != NULL)) {
      // Play the notification sounds
      soundClipPlay(sc_notification, scb_notification);
      broadcastString("4", "0");
      animation_on = TRUE;
      return 0;
  }

  // Chat bubble
  else if ((strstr(voice_str, "WHAT") != NULL) && (strstr(voice_str, "UP") != NULL)) {
    broadcastSpeech("1", "Hello! My name<br>is <b>Mr. Truman!</b>");
    animation_on = TRUE;
    return 0;
  }

  else if((strstr(voice_str, "FIDGET") != NULL) && (strstr(voice_str, "SPINNER") != NULL)){
    broadcastString("9", "1");
    return 0;
  }

  else if((strstr(voice_str, "ARE") != NULL) && (strstr(voice_str, "YOU") != NULL) && (strstr(voice_str, "HOT") != NULL)){
    broadcastSpeech("2", "<b>NEVER</b> hot...");
    animation_on = TRUE; 
    return 0;
  }

  else if((strstr(voice_str, "DO") != NULL) && (strstr(voice_str, "MATH") != NULL)){
    broadcastSpeech("3", "2 + 2 = 4 - 1 = 3<br><b>QUICK MATHS</b>");
    animation_on = TRUE; 
    return 0;
  }

  else if(strstr(voice_str, "RAP") != NULL){
    broadcastSpeech("4", "skrrrahh pap pap ka-ka-ka<br><br>");
    animation_on = TRUE; 
    return 0;
  }

  else if(strstr(voice_str, "SUPER") != NULL && (strstr(voice_str, "SAYIAN") != NULL)) {
    broadcastString("10", "0");
    return 0;
  }

  else if((strstr(voice_str, "DID") != NULL) && (strstr(voice_str, "YOU") != NULL) && (strstr(voice_str, "DO") != NULL) && (strstr(voice_str, "IT") != NULL)) {
    broadcastSpeech("5", "I did <i>nottttt</i> ....");
    animation_on = TRUE;
    return 0;
  }

  broadcastSpeech("0", "???");
  animation_on = TRUE;  
  return -1;
  
}
