#ifndef _MPU_6050_H_
#define _MPU_6050_H_



#include "MKL25Z4.h"



#define MPU_ADDRESS   0x68



extern uint8_t gyro_data[14];
extern int16_t gyro_x, gyro_y, gyro_z, temp, acc_x, acc_y, acc_z;

void InitMPU6050(void);
void readMPU6050(void);

#endif
