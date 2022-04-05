#include "MKL25Z4.h"
#include "sonar.h"
#include "gpio.h"
#include "sensors.h"
#include "clock.h"
#include <stdbool.h>

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

volatile bool isInt = false;
volatile uint32_t currTick;
volatile uint32_t prevTick;
volatile uint32_t ticksDuration;

void initSonar(void) {
  SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
  
  PORTA->PCR[TRIGGER_PIN] &= ~(PORT_PCR_MUX_MASK);
  PORTA->PCR[ECHO_PIN] &= ~(PORT_PCR_MUX_MASK);
  
  PORTA->PCR[TRIGGER_PIN] |= PORT_PCR_MUX(1);
  PORTA->PCR[ECHO_PIN] &= ~PORT_PCR_PS_MASK;
  PORTA->PCR[ECHO_PIN] |= PORT_PCR_MUX(1) | PORT_PCR_IRQC(0x0A) | PORT_PCR_PE_MASK; // interrupt on falling edge
  
  GPIOA->PDDR |= MASK(TRIGGER_PIN);
  GPIOA->PDDR &= ~MASK(ECHO_PIN);
  
  GPIOSetOutput(PORTA, TRIGGER_PIN, LOW);
  
  //NVIC_SetPriority(PORTA_IRQn, 2);
  //NVIC_ClearPendingIRQ(PORTA_IRQn);
  //isInt = false;
  //NVIC_EnableIRQ(PORTA_IRQn);
}



void PORTA_IRQHandler(void) {
  ticksDuration = osKernelGetSysTimerCount() - prevTick;
  isInt = true;
  
  NVIC_ClearPendingIRQ(PORTA_IRQn);
  PORTA->ISFR |= MASK(ECHO_PIN);
  //NVIC_DisableIRQ(PORTA_IRQn);
}

