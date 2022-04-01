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
volatile uint8_t data;

osEventFlagsId_t 
	flagForward, flagBackward, flagRight, flagLeft, flagStop,
	flagRunningSound, flagEndingSound, 
	flagTurnRightForward, flagTurnLeftForward, flagTurnRightBackWard, flagTurnLeftBackward
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
		osEventFlagsWait(flagForward, 0x01, osFlagsWaitAny, osWaitForever);
		forward();
	}
	
}

void tBackward(void *argument) {
	
	for (;;) {
		osEventFlagsWait(flagBackward, 0x01, osFlagsWaitAny, osWaitForever);
		backward();
	}
	
}

void tRight(void *argument) {
	
	for (;;) {
		osEventFlagsWait(flagRight, 0x01, osFlagsWaitAny, osWaitForever);
		right();
	}
	
}

void tLeft(void *argument) {
	
	for (;;) {
		osEventFlagsWait(flagLeft, 0x01, osFlagsWaitAny, osWaitForever);
		left();
	}
	
}

void tStop(void *argument) {
	
	for (;;) {
		osEventFlagsWait(flagStop, 0x01, osFlagsWaitAny, osWaitForever);
		

		stop();
	}
	
}

void tTurnRightForward(void *argument) {

	for (;;) {
		osEventFlagsWait(flagTurnRightForward, 0x01, osFlagsWaitAny, osWaitForever);
		turnRightForward();
	}
}

void tTurnRightBackward(void *argument) {

	for (;;) {
		osEventFlagsWait(flagTurnRightBackWard, 0x01, osFlagsWaitAny, osWaitForever);
		turnRightBackward();
	}
}

void tTurnLefttForward(void *argument) {

	for (;;) {
		osEventFlagsWait(flagTurnLeftForward, 0x01, osFlagsWaitAny, osWaitForever);
		turnLeftForward();
	}
}

void tTurnLeftBackward(void *argument) {

	for (;;) {
		osEventFlagsWait(flagTurnLeftBackward, 0x01, osFlagsWaitAny, osWaitForever);
		turnLeftBackward();
	}
}

void tRunningSound(void *argument) {
	
	for (;;) {
		osEventFlagsWait(flagRunningSound, 0x01, osFlagsWaitAny, osWaitForever);
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
		
		if (data == 0x30) { // forward
			osEventFlagsSet(flagForward, 0x01);
		} else if (data == 0x31) { // backward
			osEventFlagsSet(flagBackward, 0x01);
		} else if (data == 0x32) { // left
			osEventFlagsSet(flagLeft, 0x01);
		} else if (data == 0x33) { // right
			osEventFlagsSet(flagRight, 0x01);
		} else if (data == 0x34) { // stop
			
			osEventFlagsClear(flagForward, 0x01);
			osEventFlagsClear(flagBackward, 0x01);
			osEventFlagsClear(flagRight, 0x01);
			osEventFlagsClear(flagLeft, 0x01);

			osEventFlagsClear(flagTurnRightForward, 0x01);
			osEventFlagsClear(flagTurnLeftForward, 0x01);
			osEventFlagsClear(flagTurnRightBackWard, 0x01);
			osEventFlagsClear(flagTurnLeftBackward, 0x01);
		
			osEventFlagsSet(flagStop, 0x01);
			
		} else if (data == 0x35) {
			osEventFlagsClear(flagForward, 0x01);
		} else if (data == 0x36) {
			osEventFlagsClear(flagBackward, 0x01);
		} else if (data == 0x37) {
			osEventFlagsClear(flagLeft, 0x01);
		} else if (data == 0x38) {
			osEventFlagsClear(flagRight, 0x01);
		} else if (data == 0x39) {
			osEventFlagsSet(flagTurnRightForward, 0x01);
		} else if (data == 0x40) {
			osEventFlagsSet(flagTurnLeftForward, 0x01);
		} else if (data == 0x41) {
			osEventFlagsSet(flagTurnRightBackWard, 0x01);
		} else if (data == 0x42) {
			osEventFlagsSet(flagTurnLeftBackward, 0x01);
		}
		

	}
}

int main (void) {
	
  // System Initialization
  SystemCoreClockUpdate();
	initMotor();
	initSound();
	offLED();
	
	flagForward = osEventFlagsNew(NULL);
	flagRight = osEventFlagsNew(NULL);
	flagBackward = osEventFlagsNew(NULL);
	flagLeft = osEventFlagsNew(NULL);
	flagStop = osEventFlagsNew(NULL);
	flagRunningSound = osEventFlagsNew(NULL);

	flagTurnRightForward = osEventFlagsNew(NULL);
	flagTurnLeftForward = osEventFlagsNew(NULL);
	flagTurnRightBackWard = osEventFlagsNew(NULL);
	flagTurnLeftBackward = osEventFlagsNew(NULL);

  UARTInit(UART2, PIN_PAIR_3, 9600, true);
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(app_main, NULL, NULL);    // Create application main thread
	
	osThreadNew(tForward, NULL, NULL);
	osThreadNew(tRight, NULL, NULL);
	osThreadNew(tBackward, NULL, NULL);
	osThreadNew(tLeft, NULL, NULL);
	osThreadNew(tStop, NULL, NULL);

	osThreadNew(tTurnRightForward, NULL, NULL);
	osThreadNew(tTurnLefttForward, NULL, NULL);
	osThreadNew(tTurnRightBackward, NULL, NULL);
	osThreadNew(tTurnLeftBackward, NULL, NULL);
	
	osThreadNew(tRunningSound, NULL, NULL);
	
  osKernelStart();                      // Start thread execution
  for (;;) {}
}