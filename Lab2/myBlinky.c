#include "MKL25Z4.h"   // Device header
unsigned int counter = 0;

#define RED_LED 		18 	// PortB Pin 18
#define GREEN_LED 	19 	// PortB Pin 19
#define BLUE_LED 		1 	// PortD Pin 1
#define MASK(x) 		(1 << (x))

void InitGPIO(void) {
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

/* Delay Function */
static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

void flashRed(void) {
	PTD->PDOR |= MASK(BLUE_LED);
	PTB->PDOR |= MASK(GREEN_LED);
	PTB->PDOR &= ~MASK(RED_LED);
}

void flashGreen(void) {
	PTB->PDOR |= MASK(RED_LED);
	PTD->PDOR |= MASK(BLUE_LED);
	PTB->PDOR &= ~MASK(GREEN_LED);
}

void flashBlue (void) {
	PTB->PDOR |= (MASK(GREEN_LED) | MASK(RED_LED));
	PTD->PDOR &= ~MASK(BLUE_LED);
}

/* MAIN FUNCTION */

int main(void) {
	SystemCoreClockUpdate();
	InitGPIO();
	while(1) {
		PTD->PDOR |= MASK(BLUE_LED);
		PTB->PDOR |= MASK(GREEN_LED);
		PTB->PDOR &= ~MASK(RED_LED);
		delay(0xF00000);
		PTB->PDOR |= MASK(RED_LED);
		PTD->PDOR |= MASK(BLUE_LED);
		PTB->PDOR &= ~MASK(GREEN_LED);
		delay(0xF00000);
		PTB->PDOR |= (MASK(GREEN_LED) | MASK(RED_LED));
		PTD->PDOR &= ~MASK(BLUE_LED);
		delay(0xF00000);
	}
}
