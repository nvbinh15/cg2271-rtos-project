#include "gpio.h"
#include "MKL25Z4.h"


void GPIOInitOutput(PORT_Type* GPIOPort, int GPIOPin) {
  GPIO_Type* GPIO;
  
  // enable clock and set GPIO port base
  if (GPIOPort == PORTA) {
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    GPIO = GPIOA;
  }
  else if (GPIOPort == PORTB) {
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    GPIO = GPIOB;
  }
  else if (GPIOPort == PORTC) {
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    GPIO = GPIOC;
  }
  else if (GPIOPort == PORTD) {
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    GPIO = GPIOD;
  }
  else if (GPIOPort == PORTE) {
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    GPIO = GPIOE;
  }
  
  GPIOPort->PCR[GPIOPin] &= ~(PORT_PCR_MUX_MASK);
  GPIOPort->PCR[GPIOPin] |= PORT_PCR_MUX(1);
  
  GPIO->PDDR |= MASK(GPIOPin);
  //else {
  //  #error invalid gpio port
  //}
}

void GPIOSetOutput(PORT_Type* GPIOPort, int GPIOPin, GPIO_state_t state) {
  GPIO_Type* GPIO;
  
  if (GPIOPort == PORTA) {
    GPIO = GPIOA;
  }
  else if (GPIOPort == PORTB) {
    GPIO = GPIOB;
  }
  else if (GPIOPort == PORTC) {
    GPIO = GPIOC;
  }
  else if (GPIOPort == PORTD) {
    GPIO = GPIOD;
  }
  else if (GPIOPort == PORTE) {
    GPIO = GPIOE;
  }
  
  switch (state) {
    case HIGH:
      GPIO->PDOR |= MASK(GPIOPin);
      break;
    case LOW:
      GPIO->PDOR &= ~(MASK(GPIOPin));
      break;
  }
}

/* Initialize LEDs */
void InitLED(void) {
	// Enable Clock to PORTB and PORTD
	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));

	// Configure MUX settings to make all 3 pins GPIO
	PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);
	PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
	PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);

	// Set Data Direction Registers for PortB and PortD
	PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
	PTD->PDDR |= MASK(BLUE_LED);
}

/* Turn off all the LEDs */
void offLED(void) {
	PTB->PDOR |= (MASK(GREEN_LED) | MASK(RED_LED));
	PTD->PDOR |= MASK(BLUE_LED);
}

