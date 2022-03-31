#ifndef _GPIO_H_
#define _GPIO_H_

#include "MKL25Z4.h"

#define MASK(x)  (1 << (x))
#define RED_LED 		18 	// PortB Pin 18
#define GREEN_LED 	19 	// PortB Pin 19
#define BLUE_LED 		1 	// PortD Pin 1


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

void turnRight(void);

void InitLED(void);
void offLED(void);

#endif
