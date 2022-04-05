/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

#define RED_LED 		18 	// PortB Pin 18
#define GREEN_LED 	19 	// PortB Pin 19
#define BLUE_LED 		1 	// PortD Pin 1
#define MASK(x) 		(1 << (x))

#define led_on 	1
#define led_off 0

#define MSG_COUNT 3

osThreadId_t redLED_Id, greenLED_Id, blueLED_Id, control_Id;
osEventFlagsId_t led_flag;

typedef struct {
	uint8_t cmd;
	uint8_t data;
} myDataPkt;

osMessageQueueId_t redMsg, greenMsg, blueMsg;

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

/* Delay Routine */
static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
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
 * Application led_red thread
 *---------------------------------------------------------------------------*/
void led_red_thread (void *argument) {
 
  /*
  for (;;) {
		//osThreadFlagsWait(0x00000001, osFlagsWaitAny, osWaitForever);  // using thread flags
		//osEventFlagsWait(led_flag, 0x0001, osFlagsWaitAll, osWaitForever);	// using event flags
		
		ledControl(RED_LED, led_on);
		osDelay(1000);
		ledControl(RED_LED, led_off);
		osDelay(1000);
	}
	*/
	
	// Using message queue
	myDataPkt myRxData;
	for (;;) {
		osMessageQueueGet(redMsg, &myRxData, NULL, osWaitForever);
		if (myRxData.cmd == 0x01 && myRxData.data == 0x01) {
			ledControl(RED_LED, led_on);
			osDelay(1000);
			ledControl(RED_LED, led_off);
			osDelay(1000);			
		}
	}
	
}

/*----------------------------------------------------------------------------
 * Application led_green thread
 *---------------------------------------------------------------------------*/
void led_green_thread (void *argument) {
 
  /*
  for (;;) {
		//osThreadFlagsWait(0x00000001, osFlagsWaitAny, osWaitForever);  // using thread flags
		//osEventFlagsWait(led_flag, 0x0010, osFlagsWaitAll, osWaitForever);	// using event flags
		
		ledControl(GREEN_LED, led_on);
		osDelay(1000);
		ledControl(GREEN_LED, led_off);
		osDelay(1000);
	}
	*/
	
	// Using message queue
	myDataPkt myRxData;
	for (;;) {
		osMessageQueueGet(greenMsg, &myRxData, NULL, osWaitForever);
		if (myRxData.cmd == 0x01 && myRxData.data == 0x01) {
			ledControl(GREEN_LED, led_on);
			osDelay(1000);
			ledControl(GREEN_LED, led_off);
			osDelay(1000);			
		}
	}
}

/*----------------------------------------------------------------------------
 * Application led_blue thread
 *---------------------------------------------------------------------------*/
void led_blue_thread (void *argument) {
 
  /*
  for (;;) {
		//osThreadFlagsWait(0x00000001, osFlagsWaitAny, osWaitForever);  // using thread flags
		//osEventFlagsWait(led_flag, 0x0100, osFlagsWaitAll, osWaitForever);	// using event flags
		
		ledControl(BLUE_LED, led_on);
		osDelay(1000);
		ledControl(BLUE_LED, led_off);
		osDelay(1000);
	}
	*/
	
	// Using message queue
	myDataPkt myRxData;
	for (;;) {
		osMessageQueueGet(blueMsg, &myRxData, NULL, osWaitForever);
		if (myRxData.cmd == 0x01 && myRxData.data == 0x01) {
			ledControl(BLUE_LED, led_on);
			osDelay(1000);
			ledControl(BLUE_LED, led_off);
			osDelay(1000);			
		}
	}
}

/*----------------------------------------------------------------------------
 * Application control thread
 *---------------------------------------------------------------------------*/
void control_thread (void *argument) {
	
	myDataPkt myData;
		
	myData.cmd = 0x01;
	myData.data = 0x01;
	
	for (;;) {
		/* Using thread flags
		osThreadFlagsSet(redLED_Id, 0x0000001);
		osDelay(1000);
		osThreadFlagsSet(greenLED_Id, 0x0000001);
		osDelay(1000);
		osThreadFlagsSet(blueLED_Id, 0x0000001);
		osDelay(1000);	
		*/
		
		/* Using event flags
		osEventFlagsSet(led_flag, 0x0001);
		osDelay(1000);
		osEventFlagsSet(led_flag, 0x0010);
		osDelay(1000);
		osEventFlagsSet(led_flag, 0x0100);
		osDelay(1000);	
		*/
		
		// Using message queue
		osMessageQueuePut(redMsg, &myData, NULL, 0);
		osDelay(2000);
		
		osMessageQueuePut(greenMsg, &myData, NULL, 0);
		osDelay(2000);
		
		osMessageQueuePut(blueMsg, &myData, NULL, 0);
		osDelay(2000);
		
		osMessageQueuePut(redMsg, &myData, NULL, 0);		
		osMessageQueuePut(greenMsg, &myData, NULL, 0);
		osMessageQueuePut(blueMsg, &myData, NULL, 0);
		osDelay(2000);
		
	}
}

/* Turn off all the LEDs */
void offLED(void) {
	PTB->PDOR |= (MASK(GREEN_LED) | MASK(RED_LED));
	PTD->PDOR |= MASK(BLUE_LED);
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	InitGPIO();
	offLED();
  // ...
	
	led_flag = osEventFlagsNew(NULL);		// Create the event flag
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  redLED_Id = osThreadNew(led_red_thread, NULL, NULL);    	// Create application led_red thread
  greenLED_Id = osThreadNew(led_green_thread, NULL, NULL);    // Create application led_green thread
  blueLED_Id = osThreadNew(led_blue_thread, NULL, NULL);    // Create application led_blue thread	
	control_Id = osThreadNew(control_thread, NULL, NULL);
	
	redMsg = osMessageQueueNew(MSG_COUNT, sizeof(myDataPkt), NULL);
	greenMsg = osMessageQueueNew(MSG_COUNT, sizeof(myDataPkt), NULL);
	blueMsg = osMessageQueueNew(MSG_COUNT, sizeof(myDataPkt), NULL);
	
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
