#include "commands.h"

char* _red = "red";
char* _green = "green";
char* _blue = "blue";
char* _yellow = "yellow";
char* testType = "0";
char* testValue = "0";

int commandDetect(char const* voice_str)
{
  char* detect;

  detect = strstr(voice_str, _red);
  if (detect != NULL) {
    broadcastString(testType, _red);
    return 0;
  }
  
  detect = strstr(voice_str, _green);
  if (detect != NULL) {
    broadcastString(testType, _green);
    return 0;
  }

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
  
  printf("NOT FOUND\n");
  return -1;
}
