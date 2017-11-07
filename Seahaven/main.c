#include "main.h"

extern server_t* g_server;

static int status;
static int busy;

void webData( int type, char* value) {
  //  printf("Type: %d\nValue: %s\n\n", type, value);
  busy = 0;
  printf("busy = %d\n", busy);
  switch(type) {
  case 0:
    animationStatus(value);
    break;
  case 1:
    updateLED(value);
    break;
  case 5:
    takePhoto();
    break;
  default:
    printf("Not a valid type! [%d]\n", type);
    break;
  }
}

void animationStatus( char* status ) {
  switch(atoi(status)) {
  case 0:
    printf("Starting Lights on animation\n");
    break;
  case 1:
    printf("Ending Lights on animation\n");
    break;
  default:
    printf("Not a valid animation\n");
    break;
  }
}

void updateLED( char* rgb ) {
  const char s[2] = ",";

  // major assumption rgb is valid numbers
  uint8_t red = (uint8_t)atoi(strtok(rgb, s));
  uint8_t green =(uint8_t)atoi(strtok(NULL, s));
  uint8_t blue = (uint8_t)atoi(strtok(NULL, s));

  printf("Color:\n\tRed: %d\n\tGreen: %d\n\tBlue: %d\n", red, green, blue);
    
}

void takePhoto( void ) {
  printf("Taking Photo\n");
}

int main ( int argc, char* argv[] ) {

  g_server = (server_t*)malloc(sizeof(server_t));
  g_server->port = 6419;
  g_server->onData = webData;

  startServer();

  while(1) {}

  // hardwareTests();
  printf("main\n");
}
