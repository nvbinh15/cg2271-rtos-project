/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "MKL25Z4.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "sound.h"
 
void delay (unsigned long delay);
volatile uint8_t data = 0;
volatile uint8_t prev_data = 0;

osEventFlagsId_t 
	flagForward, flagBackward, flagRight, flagLeft, flagStop,
	flagRunningSound
;


/* Delay routine */
void delay(unsigned long delay) {
  while(delay) {
    __ASM("NOP");
    delay--;
  }
}


void UART2_IRQHandler(void) {
  NVIC_ClearPendingIRQ(UART2_IRQn);
	
  if (UART2->S1 & UART_S1_RDRF_MASK) {
    data = UART2->D;
  }
}

void tForward(void *argument) {
	
	for (;;) {
		osEventFlagsWait(flagForward, 0x0001, osFlagsNoClear, osWaitForever);
		forward();
	}
	
}

void tBackward(void *argument) {
	
	for (;;) {
		osEventFlagsWait(flagBackward, 0x0001, osFlagsNoClear, osWaitForever);
		backward();
	}
	
}

void tRight(void *argument) {
	
	for (;;) {
		osEventFlagsWait(flagRight, 0x0001, osFlagsNoClear, osWaitForever);
		right();
	}
	
}

void tLeft(void *argument) {
	
	for (;;) {
		osEventFlagsWait(flagLeft, 0x0001, osFlagsNoClear, osWaitForever);
		left();
	}
	
}

void tStop(void *argument) {
	
	for (;;) {
		osEventFlagsWait(flagStop, 0x0001, osFlagsNoClear, osWaitForever);
		/*
		osEventFlagsClear(flagForward, 0x0001);
		osEventFlagsClear(flagBackward, 0x0001);
		osEventFlagsClear(flagRight, 0x0001);
		osEventFlagsClear(flagLeft, 0x0001);
		*/
		stop();
	}
	
}

void tRunningSound(void *argument) {
	
	for (;;) {
		osEventFlagsWait(flagStop, 0x01, osFlagsNoClear, osWaitForever);
		play_running_sound();
	}
}


/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {
 
  // ...

  for (;;) {
		osEventFlagsSet(flagRunningSound, 0x01);
		
		if (data != prev_data) {
			if (data == 0x30) { // forward
				osEventFlagsSet(flagForward, 0x0001);
				//forward();
			} else if (data == 0x31) { // backward
				osEventFlagsSet(flagBackward, 0x0001);
				//backward();
			} else if (data == 0x32) { // left
				osEventFlagsSet(flagLeft, 0x0001);
				//left();
			} else if (data == 0x33) { // right
				osEventFlagsSet(flagRight, 0x0001);
				//right();
			} else if (data == 0x34) { // stop
				osEventFlagsSet(flagStop, 0x0001);
				//stop();
			}	else {
				osEventFlagsSet(flagStop, 0x0001);
				//stop();
			}
			
			prev_data = data;
		}
		

	}
}

int main (void) {
	
  // System Initialization
  SystemCoreClockUpdate();
	initMotor();
	initSound();
	
	flagForward = osEventFlagsNew(NULL);
	flagRight = osEventFlagsNew(NULL);
	flagBackward = osEventFlagsNew(NULL);
	flagLeft = osEventFlagsNew(NULL);
	flagStop = osEventFlagsNew(NULL);
	flagRunningSound = osEventFlagsNew(NULL);

  UARTInit(UART2, PIN_PAIR_3, 9600, true);
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(app_main, NULL, NULL);    // Create application main thread
	
	osThreadNew(tForward, NULL, NULL);
	osThreadNew(tRight, NULL, NULL);
	osThreadNew(tBackward, NULL, NULL);
	osThreadNew(tLeft, NULL, NULL);
	osThreadNew(tStop, NULL, NULL);
	
	osThreadNew(tRunningSound, NULL, NULL);
	
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
