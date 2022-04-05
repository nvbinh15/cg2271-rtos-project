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
#include "motor.h"

// sensors related files
#include "mpu6050.h"
#include "sonar.h"
#include "sensors.h"
#include "autorun.h"
 
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
		turnRightForward(50);
	}
}

void tTurnRightBackward(void *argument) {

	for (;;) {
		osEventFlagsWait(flagTurnRightBackWard, 0x01, osFlagsWaitAny, osWaitForever);
		turnRightBackward(50);
	}
}

void tTurnLefttForward(void *argument) {

	for (;;) {
		osEventFlagsWait(flagTurnLeftForward, 0x01, osFlagsWaitAny, osWaitForever);
		turnLeftForward(50);
	}
}

void tTurnLeftBackward(void *argument) {

	for (;;) {
		osEventFlagsWait(flagTurnLeftBackward, 0x01, osFlagsWaitAny, osWaitForever);
		turnLeftBackward(50);
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
      
      // auto run related
      osEventFlagsClear(flagAutoRun, 0x01);
		
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
		} else if (data == 0x43) {  // auto run
      osEventFlagsSet(flagAutoRun, 0x01);
    }
		

	}
}

// sensor related callback (50hz)
static void timerStart(void) {
  osTimerId_t sensorCallbackId;
  
  if (sensorCallbackId != NULL) {
    osStatus_t timerStatus = osTimerStart(sensorCallbackId, 20U);
    
    if (timerStatus != osOK) {
      GPIOSetOutput(PORTB, 19, LOW);
      UARTTransmit(UART2, '!');
    }
  }
}

void tTimerStart(void *argument) {
  timerStart();
  for(;;) {}
}

int main (void) {
	
  // System Initialization
  SystemCoreClockUpdate();
	MotorsInit();
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
  
  // auto run flag
  flagAutoRun = osEventFlagsNew(NULL);

  UARTInit(UART2, PIN_PAIR_3, 9600, true);
  I2C0Init();
  delay(0x80000);
  InitMPU6050();
  initSonar();
  
  calibrateMPU6050();
  initYawAngle();
 
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
  
  // auto run related
  osThreadNew(tTimerStart, NULL, NULL);
  osThreadNew(tAutoRun, NULL, NULL);
  
	
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
