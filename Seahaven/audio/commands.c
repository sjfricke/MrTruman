#include "commands.h"

static char* c_lights = "LIGHT";
static char* c_fire = "FIRE";
static char* c_picture = "PICTURE";
static char* c_chitchat = "WHATS UP";
static char* c_jump = "JUMP";

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

  /* 
    Light switch animation
    0 - Turn on
    1 - Turn off
    2 - Toggle Lights 
  */ 
  if (strstr(voice_str, c_lights)) {
    char* detectOn = strstr(voice_str, "ON");
    char* detectOff = strstr(voice_str, "OFF");
    char* detectRed = strstr(voice_str, "RED");
    char* detectGreen = strstr(voice_str, "GREEN");
    char* detectBlue = strstr(voice_str, "BLUE");
   
    if((detectOn != NULL) || (detectOff != NULL)){
	(detectOn != NULL) ? broadcastString("1", "0") : broadcastString("1", "1");
    } else { // Specified a color or on or off I guess - defaults to telling the house to turn lights blue
      if(detectRed != NULL){
        broadcastString("2", "255,0,0");
      } else if (detectGreen != NULL){
        broadcastString("2", "0,255,0");
      } else{
        broadcastString("2", "0,0,255");
      }
      
    }
    animation_on = TRUE;
    return 0;
  }

  /*
    Fireplace Animations
    0 - Turn on
    1 - Turn off
    2 - Toggle Fire
  */
  if (strstr(voice_str, c_fire)) {
    char* detectOn = strstr(voice_str, "ON");
    char* detectOff = strstr(voice_str, "OFF");

    if((detectOn != NULL) || (detectOff != NULL)){
    	(detectOn != NULL) ? broadcastString("3", "0") : broadcastString("3", "1");
    }
    animation_on = TRUE;
    return 0;
  }

  /*
    Taking pictures animation
  */
  if (strstr(voice_str, c_picture)) {
    broadcastString("4", "0");
    animation_on = TRUE;
    return 0;
  }

  // Chat bubble
  if (strstr(voice_str, c_chitchat)) {
    broadcastString("8", "0");
    animation_on = TRUE;
    return 0;
  }

  if(strstr(voice_str, c_jump)){
    broadcastString("9", "0");
    animation_on = TRUE;
    return 0;
  }

 
  printf("No command listed found\n");
  return -1;
}
