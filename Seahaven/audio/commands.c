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

  if(strstr(voice_str, "TRUMAN") == NULL){
    return 0;
  }

  /* 
    Light switch animation
    0 - Turn on
    1 - Turn off
    2 - Toggle Lights 
  */ 
  if ((strstr(voice_str, c_lights) != NULL) && (strstr(voice_str, "TURN") != NULL)) {
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
  if (strstr(voice_str, c_fire) != NULL) {
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
  if ((strstr(voice_str, c_picture) != NULL) || (strstr(voice_str, c_photo) != NULL)) {
    broadcastString("4", "0");
    animation_on = TRUE;
    return 0;
  }

  // Chat bubble
  if ((strstr(voice_str, "WHAT") != NULL) && (strstr(voice_str, "UP") != NULL)) {
    broadcastString("8", "Hello! My name<br>is <b>Mr. Truman!</b>");
    return 0;
  }

  if((strstr(voice_str, "FIDGET") != NULL) && (strstr(voice_str, "SPINNER") != NULL)){
    broadcastString("9", "0");
    return 0;
  }

  if((strstr(voice_str, "ARE") != NULL) && (strstr(voice_str, "YOU") != NULL) && (strstr(voice_str, "HOT") != NULL)){
    broadcastString("8", "<b>NEVER</b> hot...");
    sprintf(command, "aplay –c 0 -D plughw:0,1 ./audio/mansnothot_2.wav");
	  system(command);
    return 0;
  }
 
  printf("No command listed found\n");
  broadcastString("8", "???");
  return -1;
}
