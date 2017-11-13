#include "gpio.h"

uint16_t GpioDB410cMapping(int header_pin) {
  // only pin 23-34
  if (header_pin < 23 || header_pin > 34) {
    printf("ERROR: Pin %d does not have a GPIO pin mapped\n", header_pin);
    return 0;
  }

  switch(header_pin) {
  case 23: return 36;
  case 24: return 12;
  case 25: return 13;
  case 26: return 69;
  case 27: return 115;
  case 28: return 901; // reserved pin
  case 29: return 24;
  case 30: return 25;
  case 31: return 35;
  case 32: return 34;
  case 33: return 28;
  case 34: return 33;
  default: return 0;
  }
}

int GpioEnablePin(uint16_t pin) {
  FILE *fp;

  fp = fopen(GPIO_EXPORT_PATH, "w");
  if (fp == NULL) {
    printf("ERROR: Could not open file: %s\n", GPIO_EXPORT_PATH);
    return -1;
  }

  fprintf(fp, "%u", pin);
  fclose(fp);

  return 0;
}

int GpioDisablePin(uint16_t pin) {
  FILE *fp;

  fp = fopen(GPIO_UNEXPORT_PATH, "w");
  if (fp == NULL) {
    printf("ERROR: Could not open file: %s\n", GPIO_UNEXPORT_PATH);
    return -1;
  }

  fprintf(fp, "%u", pin);
  fclose(fp);

  return 0;
}

int GpioSetDirection(uint16_t pin, int direction) {
  FILE *fp;
  char direction_path[64];

  sprintf(direction_path, "/sys/class/gpio/gpio%u/direction", pin);

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

int GpioGetDirection(uint16_t pin) {
  FILE *fp;
  char direction_path[64];
  char direction[8];
  int return_direction;
  
  sprintf(direction_path, "/sys/class/gpio/gpio%u/direction", pin);

  fp = fopen(direction_path, "r");
  if (fp == NULL) {
    printf("ERROR: Could not open file: %s\n", direction_path);
    return -1;
  }

  fscanf(fp, "%s", direction);
  
  if (strncmp(direction, IN, 2) == 0) {
    return_direction = 0;
  } else if (strncmp(direction, OUT, 3) == 0) {
    return_direction = 1;
  } else {
    return_direction = -1;
    printf("ERROR: Unknown direction of %s\n", direction);
  }
 
  fclose(fp);
  return return_direction;
}

int GpioSetValue(uint16_t pin, int value) {
  FILE *fp;
  char value_path[64];

  sprintf(value_path, "/sys/class/gpio/gpio%u/value", pin);
  
  fp = fopen(value_path, "w");
  if (fp == NULL) {
    printf("ERROR: Could not open file: %s\n", value_path);
    return -1;
  }

  if (value == 0) {
    fputs("0", fp);
  } else if (value == 1) {
    fputs("1", fp);
  } else {
    printf("ERROR: Could not set value %d\n", value);
    fclose(fp);
    return -1;
  }

  fclose(fp);
  return 0; 
}

int GpioGetValue(uint16_t pin) {
  FILE *fp;
  char value_path[64];
  char value[8];
  
  sprintf(value_path, "/sys/class/gpio/gpio%u/value", pin);

  fp = fopen(value_path, "r");
  if (fp == NULL) {
    printf("ERROR: Could not open file: %s\n", value_path);
    return -1;
  }

  fscanf(fp, "%s", value);

  fclose(fp);

  return atoi(value);   
}
