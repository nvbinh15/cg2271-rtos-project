#include "i2c.h"
#include "MKL25Z4.h"


void I2CDelay(uint32_t t) {
  uint32_t cnt = 0;
  for (cnt = 0; cnt < t; cnt++) {
    __ASM("NOP");
  }
}

void I2C0Init(void) {
  // enable clock to Port C
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  
  // enable clock to I2C0
  SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
  
  // select alternate function 2 for PTC8 and PTC9
  PORTC->PCR[8] &= ~PORT_PCR_MUX_MASK;
  PORTC->PCR[9] &= ~PORT_PCR_MUX_MASK;
  
  PORTC->PCR[8] |= PORT_PCR_MUX(2);
  PORTC->PCR[9] |= PORT_PCR_MUX(2);
  
  // I2C clock to 600kHz
  I2C0->F = 0x14; 
  
  // enable I2C0
  I2C0->C1 = I2C_C1_IICEN_MASK;
}

void I2C0DisableAck(void) {
  I2C0->C1 |= I2C_C1_TXAK_MASK;
}

void I2C0EnableAck(void) {
  I2C0->C1 &= ~I2C_C1_TXAK_MASK;
}

void I2C0RepeatedStart(void) {
  I2C0->C1 |= I2C_C1_RSTA_MASK;
}

void I2C0EnterRXMode(void) {
  I2C0->C1 &= ~I2C_C1_TX_MASK;
}

void I2C0Start(void) {
  I2C0->C1 |= I2C_C1_TX_MASK;
	I2C0->C1 |= I2C_C1_MST_MASK;
}

void I2C0Stop(void) {
  I2C0->C1 &= ~I2C_C1_MST_MASK;
	I2C0->C1 &= ~I2C_C1_TX_MASK;
}

void I2C0Wait(void) {
  uint32_t i = 1000000;

	while(((I2C0->S & I2C_S_IICIF_MASK) == 0) && i)
	{
		i--;
	}
	I2C0->S |= I2C_S_IICIF_MASK;
}

void I2C0WriteByte(uint8_t data) {
  I2C0->D = data;
}

uint8_t I2C0ReadByte(void) {
  return (uint8_t) (I2C0->D);
}

void I2C0WriteReg(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t data) {
  I2C0Start();
	I2C0WriteByte((uint8_t)((SlaveAddress << 1) | I2C_WRITE));
	I2C0Wait();

	I2C0WriteByte(RegisterAddress);
	I2C0Wait();

	I2C0WriteByte(data);
	I2C0Wait();

	I2C0Stop();

	I2CDelay(50);
}

void I2C0ReadReg(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t n_data, uint8_t *res) {
  uint8_t i = 0;
  
  I2C0Start();
  I2C0WriteByte((uint8_t) ((SlaveAddress << 1) | I2C_WRITE));
  I2C0Wait();
  
  I2C0WriteByte(RegisterAddress);
  I2C0Wait();
  
  I2C0RepeatedStart();
  
  I2C0WriteByte((uint8_t) ((SlaveAddress << 1) | I2C_READ));
  I2C0Wait();
  
  I2C0EnterRXMode();
  I2C0EnableAck();
  
  i = I2C0ReadByte();
  I2C0Wait();
  
  for (i = 0; i < (n_data - 2); i++) {
    *res = I2C0ReadByte();
    res++;
    I2C0Wait();
  }
  
  I2C0DisableAck();
  
  *res = I2C0ReadByte();
  res++;
  I2C0Wait();
  
  I2C0Stop();
  
  *res = I2C0ReadByte();
  
  I2CDelay(50);
}
