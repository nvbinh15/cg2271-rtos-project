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


/* Initialize motors */
void initMotor(void) {
  GPIOInitOutput(PORTB, 18);
  GPIOInitOutput(PORTB, 19);
  GPIOInitOutput(PORTD, 1);
  
  GPIOInitOutput(PORTB, 0);
  GPIOInitOutput(PORTB, 1);
  GPIOInitOutput(PORTB, 2);
  GPIOInitOutput(PORTB, 3);	
}

void forward(void) {
      GPIOSetOutput(PORTB, 0, HIGH);
      GPIOSetOutput(PORTB, 1, LOW);
      GPIOSetOutput(PORTB, 2, HIGH);
      GPIOSetOutput(PORTB, 3, LOW);	
}

void backward(void) {
      GPIOSetOutput(PORTB, 0, LOW);
      GPIOSetOutput(PORTB, 1, HIGH);
      GPIOSetOutput(PORTB, 2, LOW);
      GPIOSetOutput(PORTB, 3, HIGH);	
}

void right(void) {
      GPIOSetOutput(PORTB, 0, HIGH);
      GPIOSetOutput(PORTB, 1, LOW);
      GPIOSetOutput(PORTB, 2, LOW);
      GPIOSetOutput(PORTB, 3, HIGH);	
}

void left(void) {
      GPIOSetOutput(PORTB, 0, LOW);
      GPIOSetOutput(PORTB, 1, HIGH);
      GPIOSetOutput(PORTB, 2, HIGH);
      GPIOSetOutput(PORTB, 3, LOW);	
}

void stop(void) {
      GPIOSetOutput(PORTB, 0, HIGH);
      GPIOSetOutput(PORTB, 1, HIGH);
      GPIOSetOutput(PORTB, 2, HIGH);
      GPIOSetOutput(PORTB, 3, HIGH);	
}

void turnRightForward(void) {
      GPIOSetOutput(PORTB, 0, HIGH);
      GPIOSetOutput(PORTB, 1, LOW);
      GPIOSetOutput(PORTB, 2, HIGH);
      GPIOSetOutput(PORTB, 3, HIGH);	
}

void turnLeftForward(void) {
      GPIOSetOutput(PORTB, 0, HIGH);
      GPIOSetOutput(PORTB, 1, HIGH);
      GPIOSetOutput(PORTB, 2, HIGH);
      GPIOSetOutput(PORTB, 3, LOW);	
}

void turnRightBackward(void) {
      GPIOSetOutput(PORTB, 0, LOW);
      GPIOSetOutput(PORTB, 1, HIGH);
      GPIOSetOutput(PORTB, 2, HIGH);
      GPIOSetOutput(PORTB, 3, HIGH);	
}

void turnLeftBackward(void) {
      GPIOSetOutput(PORTB, 0, HIGH);
      GPIOSetOutput(PORTB, 1, HIGH);
      GPIOSetOutput(PORTB, 2, LOW);
      GPIOSetOutput(PORTB, 3, HIGH);	
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

