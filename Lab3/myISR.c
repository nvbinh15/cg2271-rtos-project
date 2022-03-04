#include "MKL25Z4.h"

#define RED_LED 		18		// PortB pin 18
#define GREEN_LED 	19		// PortB pin 19
#define BLUE_LED 		1			// PortD pin 1
#define MASK(x) 	  (1 << (x))
#define SW_POS			6 		// PortD pin 6

char led_mapping[3][2] = {
	{0, RED_LED},
	{1, GREEN_LED},
	{2, BLUE_LED}
};
volatile int counter = 0;
volatile int led_id = 0;


void initSwitch(void) {
	// enable clock for PortD
	SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK);	
	
	/* Select GPIO and enable pull-up resistors and interrupts
	on falling edges of pin connected to switch */
	PORTD->PCR[SW_POS] |= (
			PORT_PCR_MUX(1) | 
			PORT_PCR_PS_MASK | 
			PORT_PCR_PS_MASK | 
			PORT_PCR_IRQC(0x0a)
	);
	
	// Set PORT D Switch bit to input
	PTD->PDDR &= ~MASK(SW_POS);
	
	// Enable Interrupts
	NVIC_SetPriority(PORTD_IRQn, 2);
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	NVIC_EnableIRQ(PORTD_IRQn);
}

void initLED(void) {
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

void PORTD_IRQHandler() {
	// Clear Pending IRQ
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	
	// Updating some varable /flag
	led_id++;
	counter++;
	
	if (led_id > 2) {
		led_id = 0;
	}
	
	// Clear INT Flag
	PORTD->ISFR |= MASK(SW_POS);
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

void flashBlue(void) {
	PTB->PDOR |= (MASK(GREEN_LED) | MASK(RED_LED));
	PTD->PDOR &= ~MASK(BLUE_LED);
}

void offLED(void) {
	PTB->PDOR |= (MASK(GREEN_LED) | MASK(RED_LED));
	PTD->PDOR |= MASK(BLUE_LED);
}

int main(void) {
	initSwitch();
	initLED();
	offLED();
	
	while (1) {
		if (led_id == 0) {
			flashRed();
		} else if (led_id == 1) {
			flashGreen();
		} else {
			flashBlue();
		}
	}
	
}