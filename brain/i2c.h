#ifndef _I2C_H_
#define _I2C_H_

#include "MKL25Z4.h"

#define I2C_WRITE	0	// Master write
#define I2C_READ	1	// Master read

void I2CDelay(uint32_t t);
void I2C0Init(void);
void I2C0DisableAck(void);
void I2C0EnableAck(void);
void I2C0RepeatedStart(void);
void I2C0EnterRXMode(void);
void I2C0Start(void);
void I2C0Stop(void);
void I2C0Wait(void);
void I2C0WriteByte(uint8_t data);
uint8_t I2C0ReadByte(void);
void I2C0WriteReg(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t data);
void I2C0ReadReg(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t n_data, uint8_t *res);

#endif
