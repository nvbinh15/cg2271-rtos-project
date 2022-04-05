#include "mpu6050.h"
#include "i2c.h"
#include "sensors.h"

uint8_t gyro_data[14];
int16_t gyro_x, gyro_y, gyro_z, temp, acc_x, acc_y, acc_z;

void InitMPU6050(void) {
  I2C0WriteReg(MPU_ADDRESS, 0x6B, 0x00);
  I2C0WriteReg(MPU_ADDRESS, 0x1B, 0x08);
  I2C0WriteReg(MPU_ADDRESS, 0x1C, 0x10);
  I2C0WriteReg(MPU_ADDRESS, 0x1A, 0x03);
}

void readMPU6050(void) {
  I2C0ReadReg(MPU_ADDRESS, 0x3B, 14, gyro_data);
  
  acc_x = gyro_data[0] << 8 | gyro_data[1];
  acc_y = gyro_data[2] << 8 | gyro_data[3];
  acc_z = gyro_data[4] << 8 | gyro_data[5];
  temp = gyro_data[6] << 8 | gyro_data[7];
  gyro_x = gyro_data[8] << 8 | gyro_data[9];
  gyro_y = gyro_data[10] << 8 | gyro_data[11];
  gyro_z = gyro_data[12] << 8 | gyro_data[13];
  gyro_z *= -1;
  
  if (cal_int >= SAMPLE_SIZE) {
    gyro_x -= gyro_x_cal;
    gyro_y -= gyro_y_cal;
    gyro_z -= gyro_z_cal;
  }
}
