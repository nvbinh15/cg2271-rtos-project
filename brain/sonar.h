#ifndef _SONAR_H_
#define _SONAR_H_

#include "MKL25Z4.h"
#include <stdbool.h>

#define ECHO_PIN      1
#define TRIGGER_PIN   2


extern volatile bool isInt;
extern volatile uint32_t currTick;
extern volatile uint32_t prevTick;
extern volatile uint32_t ticksDuration;

void initSonar(void);
void PORTA_IRQHandler(void);

#endif
