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
#define MASK(x) 		(1 << (x))

#define led_on 	1
#define led_off 0

const osThreadAttr_t thread_attr = {
	.priority = osPriorityNormal1
};

osMutexId_t myMutex;

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

/* Delay Routine */
static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}
 
/*----------------------------------------------------------------------------
 * Application led_red thread
 *---------------------------------------------------------------------------*/
void led_red_thread (void *argument) {
 
  // ...
  for (;;) {
		osMutexAcquire(myMutex, osWaitForever);
		
		ledControl(RED_LED, led_on);
		//delay(0x80000); 
		osDelay(1000);
		ledControl(RED_LED, led_off);
		//delay(0x80000); 
		osDelay(1000);
		
		osMutexRelease(myMutex);
	}
}

/*----------------------------------------------------------------------------
 * Application led_green thread
 *---------------------------------------------------------------------------*/
void led_green_thread (void *argument) {
 
  // ...
  for (;;) {
		osMutexAcquire(myMutex, osWaitForever);
		
		ledControl(GREEN_LED, led_on);
		//delay(0x80000); 
		osDelay(1000);
		ledControl(GREEN_LED, led_off);
		//delay(0x80000); 
		osDelay(1000);
		
		//osMutexRelease(myMutex);
	}
}
 
int main (voiSd) {
 
  // System Initialization
  SystemCoreClockUpdate();
	InitGPIO();
	offRGB();
  // ...
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
	myMutex = osMutexNew(NULL);
  osThreadNew(led_red_thread, NULL, NULL);
  //osThreadNew(led_red_thread, NULL, &thread_attr);	
	osThreadNew(led_green_thread, NULL, NULL);
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
