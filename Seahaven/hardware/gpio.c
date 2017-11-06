#include "gpio.h"

char* GpioDB410cMapping(int header_pin) {
  // only pin 23-34
  if (header_pin < 23 || header_pin > 34) {
    printf("ERROR: Pin %d does not have a GPIO pin mapped\n", header_pin);
    return NULL;
  }

  switch(header_pin) {
  case 23: return "36";
  case 24: return "12";
  case 25: return "13";
  case 26: return "69";
  case 27: return "115";
  case 28: return "901"; // reserved pin
  case 29: return "24";
  case 30: return "25";
  case 31: return "35";
  case 32: return "34";
  case 33: return "28";
  case 34: return "33";
  default: return NULL;
  }
}

int GpioEnablePin(char* pin) {
  FILE *fp;

  fp = fopen(GPIO_EXPORT_PATH, "w");
  if (fp == NULL) {
    printf("ERROR: Could not open file: %s\n", GPIO_EXPORT_PATH);
    return -1;
  }

  fputs(pin, fp);
  fclose(fp);

  return 0;
}

int GpioDisablePin(char* pin) {
  FILE *fp;

  fp = fopen(GPIO_UNEXPORT_PATH, "w");
  if (fp == NULL) {
    printf("ERROR: Could not open file: %s\n", GPIO_UNEXPORT_PATH);
    return -1;
  }

  fputs(pin, fp);
  fclose(fp);

  return 0;
}

int GpioSetDirection(char* pin, int direction) {
  FILE *fp;
  char direction_path[64];

  sprintf(direction_path, "/sys/class/gpio/gpio%s/direction", pin);

  fp = fopen(direction_path, "w");
  if (fp == NULL) {
    printf("ERROR: Could not open file: %s\n", direction_path);
    return -1;
  }

  if (direction == 0) {
    fputs(IN, fp);
  } else if (direction == 1) {
    fputs(OUT, fp);
  } else {
    printf("ERROR: Could not set direction %d\n", direction);
    fclose(fp);
    return -1;
  }
 
  fclose(fp);
  return 0; 
}

int GpioGetDirection(char* pin) {
  FILE *fp;
  char direction_path[64];
  char direction[8];
  int return_direction;
  
  sprintf(direction_path, "/sys/class/gpio/gpio%s/direction", pin);

  fp = fopen(direction_path, "r");
  if (fp == NULL) {
    printf("ERROR: Could not open file: %s\n", direction_path);
    return -1;
  }

  fscanf(fp, "%s", direction);
  
  if (strncmp(direction, IN, 2)) {
    return_direction = 0;
  } else if (strncmp(direction, OUT, 3)) {
    return_direction = 1;
  } else {
    return_direction = -1;
    printf("ERROR: Unknown direction of %s\n", direction);
  }
 
  fclose(fp);
  return return_direction;
}

int GpioSetValue(char* pin, int value) {
  FILE *fp;
  char value_path[64];

  sprintf(value_path, "/sys/class/gpio/gpio%s/value", pin);
  
  fp = fopen(value_path, "w");
  if (fp == NULL) {
    printf("ERROR: Could not open file: %s\n", value_path);
    return -1;
  }

  if (value == 0) {
    fputs(LOW, fp);
  } else if (value == 1) {
    fputs(HIGH, fp);
  } else {
    printf("ERROR: Could not set value %d\n", value);
    fclose(fp);
    return -1;
  }

  fclose(fp);
  return 0; 
}

int GpioGetValue(char* pin) {
  FILE *fp;
  char value_path[64];
  char value[8];
  int return_value;
  
  sprintf(value_path, "/sys/class/gpio/gpio%s/value", pin);

  fp = fopen(value_path, "r");
  if (fp == NULL) {
    printf("ERROR: Could not open file: %s\n", value_path);
    return -1;
  }

  fscanf(fp, "%s", value);
  
  if (strncmp(value, LOW, 1)) {
    return_value = 0;
  } else if (strncmp(value, HIGH, 1)) {
    return_value = 1;
  } else {
    return_value = -1;
    printf("ERROR: Unknown value of %s\n", value);
  }
 
  fclose(fp);
  return return_value;
}
