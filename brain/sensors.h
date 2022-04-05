#ifndef _SENSORS_H_
#define _SENSORS_H_

#include "MKL25Z4.h"

#define SAMPLE_SIZE 2500

extern int cal_int;
extern int gyro_x_cal, gyro_y_cal, gyro_z_cal;

extern float sonarDistance;
extern volatile float compass_heading, angle_yaw;
extern volatile float gyro_yaw_input;

void calibrateMPU6050(void);
void initYawAngle(void);
void readSonar(void);
void imuCallback(void *argument);
void tSonar(void *argument);
void sonarCallback(void *argument);

#endif
