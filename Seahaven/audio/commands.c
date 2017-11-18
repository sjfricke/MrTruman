#include "commands.h"

static char* c_lights = "LIGHT";
static char* c_fire = "FIRE";
static char* c_picture = "PICTURE";
static char* c_take = "TAKE";
static char* c_fan = "FAN";
static char* c_chitchat = "HOW YOU DOIN";

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

  printf("Said: %s\n", voice_str);

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

    char * broadcaststr = char * malloc(200*sizeof(char));
   
    if((detectOn != NULL) || (detectOff != NULL)){
        int onoff = (detectOn != NULL) ? 0 : 1;
		    sprintf(broadcaststr, "%d", onoff);
		    broadcastString("1", broadcaststr);
    } else { // Specified a color or on or off I guess - defaults to telling the house to turn lights blue
      if(detectRed != NULL){
        broadcastString("2", "255,0,0");
      } else if (detectGreen != NULL){
        broadcastString("2", "0,255,0");
      } else{
        broadcastString("2", "0,0,255");
      }
      
    }
    free(broadcaststr);
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

    char * broadcaststr = char * malloc(200*sizeof(char));

    if((detectOn != NULL) || (detectOff != NULL)){
        int onoff = (detectOn != NULL) ? 0 : 1;
		    sprintf(broadcaststr, "%d", onoff);
		    broadcastString("3", broadcaststr);
    }
     free(broadcaststr);
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


  if (strstr(voice_str, c_fan)) {
    broadcastString("2", "0");
    animation_on = TRUE;
    return 0;
  }

  if (strstr(voice_str, c_chitchat)) {
    broadcastString("8", "0");
    animation_on = TRUE;
    return 0;
  }



  /*

  detect = strstr(voice_str, _blue);
  if (detect != NULL) {
    broadcastString(testType, _blue);
    return 0;
  }


  if (strstr(voice_str, c_take)) {
    broadcastString("1", "0");
    animation_on = TRUE;
    return 0;
  }

  detect = strstr(voice_str, _yellow);
  if (detect != NULL) {
    broadcastString(testType, _yellow);
    return 0;
  }
  */
  
  printf("No command listed found\n");
  return -1;
}
