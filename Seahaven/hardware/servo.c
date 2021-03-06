#include "PCA9685.h"

static uint16_t servo_gpio;

// read state from file
char checkServoState() {
  FILE *servo_state_file = fopen(SERVO_STATE_FILE, "r");
  char ch;
  ch = fgetc(servo_state_file);
  fclose(servo_state_file);
  return ch;
}

// write state to file
void persistServoState(char state) {
  FILE *servo_state_file = fopen(SERVO_STATE_FILE, "w");
  fputc(state, servo_state_file);
  fclose(servo_state_file);
}

// makes file and says servo not extended if it doesnt exist
void checkServoStateFile() {
    if( access( SERVO_STATE_FILE, F_OK ) == -1 ) {
        // file doesn't exist, so lets make it
      int fd2 = open(SERVO_STATE_FILE, O_RDWR|O_CREAT, 0777);  // Originally 777 (see comments)

      if (fd2 != -1) {
        // use file descriptor
        close(fd2);
      }
      persistServoState(STATE_SERVO_RETRACTED);
    }
}

int servofanStart()
{
  // needs to set GPIO pin 12 (24 front facing) to 0 or servo wont go
  servo_gpio = GpioDB410cMapping(SERVO_GPIO_PIN);
  GpioEnablePin(servo_gpio);
  GpioSetDirection(servo_gpio, OUTPUT_PIN);
  GpioSetValue(servo_gpio, LOW);

  // Initialize I2C Bus
  PCA9685_Start();

  checkServoStateFile();
  if (checkServoState() == STATE_SERVO_EXTENDED) {
      // retract servo
      servoRetract();
  }

  return 0;
}

int servoStop()
{

 // GpioSetValue(servo_gpio, HIGH);

  // Write all off.
  I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_ON_L, 0x0);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_ON_H, 0x0);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_OFF_L, 0x0);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_OFF_H, 0x0);

  // Terminate Bus
  PCA9685_End();

  return 0;
}

int servoExtend()
{
  if (checkServoState() == STATE_SERVO_EXTENDED) {
      return 0;
  }

  // Now initialize the chip with a reset and enable the
  // gpio enable if needed (I don't think its neccessary yet)
  uint8_t temp_reg;

  I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
  I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);

  // Check if held in reset
  if ((temp_reg & (PCA9685_HELD_RESET)) != 1)
  {
    // Set into sleep mode.
    I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
    I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);
    temp_reg |= (1 << 4); // Turn off oscillator
    I2cWriteByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, temp_reg);
  }

  // Set prescale clock to 0x81 for 47Hz PWM (21.3ms period)
  I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
  I2cWriteByte(PCA9685_I2C_BUS, PCA9685_PRESCALE_REG, 0x81);

  // Turn on oscillator
  I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);
  temp_reg &= ~(1 << 4);
  temp_reg &= ~(1); // Turn off oscillator
  I2cWriteByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, temp_reg);
  // Wait for oscillator to stabilize
  usleep(600);

  // Clear reset
  I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);
  temp_reg |= (1 << 7);
  I2cWriteByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, temp_reg);
  usleep(600); // Need to wait at least 500uS before using the chip - enforced

  // Need to turn on the outputs for a 1.3ms on time for the 21.3ms period.
  // On Value = 0x0 (0,0)
  // Off Value = 0xFA *(0,FA)
  //
  // -----
  //     |
  //      -----------------
  //    1.3               21.3
  //
  I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_ON_L, 0x0);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_ON_H, 0x0);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_OFF_L, 0x2D);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_OFF_H, 0x1);

  // Wait two seconds (or however long the it takes to extend)
  usleep(TWOSECONDS_FWD);
printf("rotation done");
  // Turn off the outputs.
  I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_ON_L, 0x0);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_ON_H, 0x0);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_OFF_L, 0x0);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_OFF_H, 0x0);

  persistServoState(STATE_SERVO_EXTENDED);
  return 0;
}

int servoRetract()
{
  if (checkServoState() == STATE_SERVO_RETRACTED) {
    return 0;
  }

  PCA9685_Start();

  // Now initialize the chip with a reset and enable the
  // gpio enable if needed (I don't think its neccessary yet)
  uint8_t temp_reg;

  I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
  I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);

  // Check if held in reset
  if ((temp_reg & (PCA9685_HELD_RESET)) != 1)
  {
    // Set into sleep mode.
    I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
    I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);
    temp_reg |= (1 << 4); // Turn off oscillator
    temp_reg &= ~(1);     // Turn off oscillator
    I2cWriteByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, temp_reg);
  }

  // Set prescale clock to 0x83 for 46Hz PWM (21.7ms period)
  I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
  I2cWriteByte(PCA9685_I2C_BUS, PCA9685_PRESCALE_REG, 0x83);

  // Turn on oscillator
  I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);
  temp_reg &= ~(1 << 4);
  I2cWriteByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, temp_reg);
  // Wait for oscillator to stabilize
  usleep(600);

  // Clear reset
  I2cReadByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, &temp_reg);
  temp_reg |= (1 << 7);
  I2cWriteByte(PCA9685_I2C_BUS, PCA9685_MODE_REG, temp_reg);
  usleep(600);

  // Need to turn on the outputs for a 1.7ms on time for the 21.7ms period.
  // On Value = 0x0 (0,0)
  // Off Value = 0x141 *(1,41)
  //
  // -----
  //     |1
  //      -----------------
  //    1.7               21.7
  //
  I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_ON_L, 0x0);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_ON_H, 0x0);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_OFF_L, 0x32);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_OFF_H, 0x01);

  // Wait two seconds (or however long the it takes to extend)
  usleep(TWOSECONDS_BCK);

  // Turn off the outputs.
  // Can turn into a single write once verified.
  I2cSetSlave(PCA9685_I2C_BUS, PCA9685_SERVO_FAN_ADDRESS);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_ON_L, 0x0);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_ON_H, 0x0);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_OFF_L, 0x0);
  I2cWriteByte(PCA9685_I2C_BUS, SERVO_OFF_H, 0x0);

  persistServoState(STATE_SERVO_RETRACTED);
  return 0;
}
