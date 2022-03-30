#ifndef _GPIO_H_
#define _GPIO_H_

#include "MKL25Z4.h"

#define MASK(x)  (1 << (x))


typedef enum GPIO_state {
  HIGH,
  LOW
} GPIO_state_t;

void GPIOInitOutput(PORT_Type* GPIOPort, int GPIOPin);
void GPIOSetOutput(PORT_Type* GPIOPort, int GPIOPin, GPIO_state_t state);

void initMotor(void);
void forward(void);
void backward(void);
void right(void);
void left(void);
void stop(void);

#endif
