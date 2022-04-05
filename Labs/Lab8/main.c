/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "MKL25Z4.h"   // Device header

#define RED_LED 		18 	// PortB Pin 18
#define GREEN_LED 	19 	// PortB Pin 19
#define BLUE_LED 		1 	// PortD Pin 1
#define SW_POS			6 	// PortD pin 6
#define MASK(x) 		(1 << (x))

#define led_on 	1
#define led_off 0

osSemaphoreId_t mySem;

/* Delay Routine */
static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
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

/* Initialize switch interrupt */
void initSwitch(void) {
	// enable clock for PortD
	SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK);	// SIM->SCGC5 != (1ul << 12);
	
	/* Select GPIO and enable pull-up resistors and interrupts
	on falling edges of pin connected to switch */
	PORTD->PCR[SW_POS] |= (
			PORT_PCR_MUX(1) | 	// enable GPIO
			PORT_PCR_PE_MASK | 	// enable the pull register
			PORT_PCR_PS_MASK | 	// pull up
			PORT_PCR_IRQC(0x0a)	// IRQC = 1010 -> interrupt on falling edge
	);
	
	// Set PORT D Switch bit to input
	PTD->PDDR &= ~MASK(SW_POS); // PTD->PDDR = 0
	
	// Enable Interrupts
	NVIC_SetPriority(PORTD_IRQn, 2);	//	IRP[7] |= (1ul << 31);
	NVIC_ClearPendingIRQ(PORTD_IRQn);	
	NVIC_EnableIRQ(PORTD_IRQn);			//	NVIC_ISER
}


void PORTD_IRQHandler() {
	// Clear Pending IRQ
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	
	delay(0x80000);
	osSemaphoreRelease(mySem);
	
	// Clear INT Flag
	PORTD->ISFR |= MASK(SW_POS);
}


/* Turn off all the LEDs */
void offRGB(void) {
	PTB->PDOR |= (MASK(GREEN_LED) | MASK(RED_LED));
	PTD->PDOR |= MASK(BLUE_LED);
}

/* Control LED */
void ledControl(uint8_t LED, uint8_t status) {
	if (LED == RED_LED) {
		if (status == led_on) {
			PTB->PDOR &= ~MASK(RED_LED);
		} else if (status == led_off) {
			PTB->PDOR |= MASK(RED_LED);
		}
	} else if (LED == GREEN_LED) {
		if (status == led_on) {
			PTB->PDOR &= ~MASK(GREEN_LED);
		} else if (status == led_off) {
			PTB->PDOR |= MASK(GREEN_LED);
		}
	} else if (LED == BLUE_LED) {
		if (status == led_on) {
			PTD->PDOR &= ~MASK(BLUE_LED);
		} else if (status == led_off) {
			PTD->PDOR |= MASK(BLUE_LED);
		}
	}
}


 
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void led_red_thread (void *argument) {
 
  // ...
  for (;;) {
		osSemaphoreAcquire(mySem, osWaitForever);
		
		ledControl(RED_LED, led_on);
		delay(0x80000); 
		//osDelay(1000);
		ledControl(RED_LED, led_off);
		delay(0x80000); 
		//osDelay(1000);
		
		osSemaphoreRelease(mySem);
	}
}

void led_green_thread (void *argument) {
 
  // ...
  for (;;) {
		osSemaphoreAcquire(mySem, osWaitForever);
		
		ledControl(GREEN_LED, led_on);
		delay(0x80000); 
		//osDelay(1000);
		ledControl(GREEN_LED, led_off);
		delay(0x80000); 
		//osDelay(1000);
		
		osSemaphoreRelease(mySem);
	}
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	InitGPIO();
	delay(0x800000);
	offRGB();
  // ...
 
  osKernelInitialize();                 			// Initialize CMSIS-RTOS
  mySem = osSemaphoreNew(1, 1, NULL);
	osThreadNew(led_red_thread, NULL, NULL);    // Create new led_red thread
	osThreadNew(led_green_thread, NULL, NULL);	// Create new led_green main thread
  osKernelStart();                      			// Start thread execution
  for (;;) {}
}
