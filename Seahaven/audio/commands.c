#include "commands.h"

static char* c_lights = "LIGHT";

static char* c_picture = "PICTURE";
static char* c_take = "TAKE";
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

  detect = strstr(voice_str, c_lights);
  if (detect != NULL) {
    broadcastString("0", "0");
    return 0;
  }
  
  
  detect = strstr(voice_str, c_take);
  if (detect != NULL) {
    broadcastString("1", "0");
    return 0;
  }

  detect = strstr(voice_str, c_picture);
  if (detect != NULL) {
    broadcastString("1", "1");
    return 0;
  }
  /*

  detect = strstr(voice_str, _blue);
  if (detect != NULL) {
    broadcastString(testType, _blue);
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
