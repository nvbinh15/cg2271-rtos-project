#include "MKL25Z4.h"

// 48MHz core clk freq and 24 MHz Bus clk freq

#define BAUD_RATE 9600
#define UART_TX_PORTE22	22
#define UART_RX_PORTE23	23
#define UART2_INT_PRIO	128

#define RED_LED 		18 	// PortB Pin 18
#define GREEN_LED 	19 	// PortB Pin 19
#define BLUE_LED 		1 	// PortD Pin 1
#define MASK(x) 		(1 << (x))

/* Init UART2 */
void initUART2 (uint32_t baud_rate) {
	uint32_t divisor, bus_clock;
	
	// Enable clock to UART2 and PORTE
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;	// SIM->SCGC4 |= (1UL << 12);
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;	// SIM->SCGC5 |= (1ul << 13);
	
	// Connect UART pins for PTE22, PTE23 -> ALT4 (docs table page 161)
	PORTE->PCR[UART_TX_PORTE22] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_TX_PORTE22] |= PORT_PCR_MUX(4);		// set PCR[10:8] to 100
	
	PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4);	
	
	// Ensure Tx and Rx are disabled before configuration
	UART2->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));	//	UART->C2[3:2] = 00;
	
	// Set Baud Rate to the desired value
	bus_clock = (DEFAULT_SYSTEM_CLOCK) / 2;		// bus clock is half of system clock (default)
	divisor = bus_clock / (baud_rate * 16);		// set divisor
	UART2->BDH = UART_BDH_SBR(divisor >> 8);	// load 8 most significant bits of divisor
	UART2->BDL = UART_BDL_SBR(divisor);			// load 8 least significant bits of divisor
	
	// No parity, 8-bits
	UART2->C1 = 0;	// PE = 0 -> no parity; M = 0 -> 8-bits
	UART2->S2 = 0;	
	UART2->C3 = 0;
	
	//NVIC_SetPriority(UART2_IRQn, UART2_INT_PRIO);
	//NVIC_ClearPendingIRQ(UART2_IRQn);
	//NVIC_EnableIRQ(UART2_IRQn);
	
	// Enable Tx and Rx
	UART2->C2 |= ((UART_C2_TE_MASK) | (UART_C2_RE_MASK)); //	UART->C2[3:2] = 11;
}

/* UART Transmit Poll */
void UART2_Transmit_Poll(uint8_t data) {
	// Wait until transmit data register is full
	while (!(UART2->S1 & UART_S1_TDRE_MASK));	// wait until TDRE == 1
	UART2->D = data;
}	

/* UART2 Receive Poll */
uint8_t UART2_Receive_Poll(void) {
	// Wait until receive data register is full
	while (!(UART2->S1 & UART_S1_RDRF_MASK));	// wait until RDRF == 1
	return (UART2->D);
}

/* Delay Routine */
static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

/* Turn on the Red LED*/
void flashRed(void) {
	PTD->PDOR |= MASK(BLUE_LED);
	PTB->PDOR |= MASK(GREEN_LED);
	PTB->PDOR &= ~MASK(RED_LED);
}

/* Turn on the Green LED*/
void flashGreen(void) {
	PTB->PDOR |= MASK(RED_LED);
	PTD->PDOR |= MASK(BLUE_LED);
	PTB->PDOR &= ~MASK(GREEN_LED);
}

/* Turn on the Blue LED*/
void flashBlue (void) {
	PTB->PDOR |= (MASK(GREEN_LED) | MASK(RED_LED));
	PTD->PDOR &= ~MASK(BLUE_LED);
}

/* Turn off all the LEDs */
void offLED(void) {
	PTB->PDOR |= (MASK(GREEN_LED) | MASK(RED_LED));
	PTD->PDOR |= MASK(BLUE_LED);
}

/* Initialize GPIO pins */
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

/* Main Function */
int main(void) {
	
	uint8_t rx_data;
	InitGPIO();
	SystemCoreClockUpdate();
	initUART2(BAUD_RATE);
	
	while (1) {
		/* Rx and Tx */
		
		//UART2_Transmit_Poll(rx_data);
		rx_data = UART2_Receive_Poll();
		
		switch (rx_data) {
			case 0x30: offLED(); break;
			case 0x31: flashRed(); break;
			case 0x32: offLED(); break;
			case 0x33: flashGreen(); break;
			case 0x34: offLED(); break;
			case 0x35: flashBlue(); break;
			default: offLED(); break;
		}

		//UART2_Transmit_Poll(rx_data);
		//rx_data = UART2_Receive_Poll();

	}
}

