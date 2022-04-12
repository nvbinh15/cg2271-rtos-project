#include "MKL25Z4.h"
#include "mpu6050.h"
#include "sonar.h"
#include "sensors.h"
#include "gpio.h"
#include "clock.h"
#include "uart.h"

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

int cal_int = 0;
int gyro_x_cal, gyro_y_cal, gyro_z_cal;

float sonarDistance;

volatile float compass_heading, angle_yaw;
volatile float gyro_yaw_input;

static void sensorDelay(uint32_t time) {
  while (time--) 
    __ASM("NOP");
}

void calibrateMPU6050(void) {
  for (cal_int = 0; cal_int < SAMPLE_SIZE; cal_int++) {
    if (cal_int % 25 == 0) 
      GPIOToggleOutput(PORTD, 1);
    readMPU6050();
    gyro_x_cal += gyro_x;
    gyro_y_cal += gyro_y;
    gyro_z_cal += gyro_z;
    
    sensorDelay(50000);
  }
  GPIOSetOutput(PORTD, 1, HIGH);
  
  gyro_x_cal /= SAMPLE_SIZE;
  gyro_y_cal /= SAMPLE_SIZE;
  gyro_z_cal /= SAMPLE_SIZE;
}

void initYawAngle(void) {
  angle_yaw = 0;
}

void readSonar(void) {
  GPIOSetOutput(PORTA, TRIGGER_PIN, HIGH);
    kernelDelayUs(10);
    GPIOSetOutput(PORTA, TRIGGER_PIN, LOW);
    
    while (!(GPIOA->PDIR & MASK(ECHO_PIN)));
    uint32_t prevSonarTick = osKernelGetSysTimerCount();
    uint32_t currSonarTick = 0;
    
    while (GPIOA->PDIR & MASK(ECHO_PIN)) {
      currSonarTick = osKernelGetSysTimerCount();
    }
    uint32_t duration = currSonarTick - prevSonarTick;
    sonarDistance = ((duration * 1.0f) / (osKernelGetSysTimerFreq() / 1000000)) * 0.034f / 2;
    PORTA->ISFR |= MASK(ECHO_PIN);
}

void tSonar(void *argument) {
  for (;;) {
    GPIOSetOutput(PORTA, TRIGGER_PIN, HIGH);
    kernelDelayUs(10);
    GPIOSetOutput(PORTA, TRIGGER_PIN, LOW);
    
    while (!(GPIOA->PDIR & MASK(ECHO_PIN)));
    uint32_t prevSonarTick = osKernelGetSysTimerCount();
    uint32_t currSonarTick = 0;
    
    while (GPIOA->PDIR & MASK(ECHO_PIN)) {
      currSonarTick = osKernelGetSysTimerCount();
    }
    uint32_t duration = currSonarTick - prevSonarTick;
    sonarDistance = ((duration * 1.0f) / (osKernelGetSysTimerFreq() / 1000000)) * 0.034f / 2;
    PORTA->ISFR |= MASK(ECHO_PIN);
  }
}

void imuCallback(void *argument) {
  readMPU6050();
  
  gyro_yaw_input = (gyro_yaw_input * 0.7f) + (((float)gyro_z / 65.5f) * 0.3f);      //Gyro pid input is deg/sec.
  //angle_yaw += (float) gyro_z * 0.00030534f;
  angle_yaw += (float) gyro_yaw_input * 0.02;
  
}

void sonarCallback(void *argument) {
  readSonar();
}
