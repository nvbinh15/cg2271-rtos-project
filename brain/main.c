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


#include "ledCycle.h"

// sensors related files
#include "mpu6050.h"
#include "sonar.h"
#include "sensors.h"
#include "autorun.h"


#define DUTY_CYCLE_TURN	30

#define FORWARD						0x30
#define BACKWARD 					0x31
#define LEFT 							0x32
#define RIGHT 						0x33
#define STOP 							0x34
#define STOP_FORWARD 			0x35
#define STOP_BACKWARD 		0x36
#define STOP_LEFT 				0x37
#define STOP_RIGHT 				0x38
#define RIGHT_FORWARD 		0x39
#define LEFT_FORWARD 			0x40
#define RIGHT_BACKWARD 		0x41
#define LEFT_BACKWARD 		0x42

#define AUTO 							0x50
#define FINISH 						0x51
 
void delay (unsigned long delay);
volatile uint8_t data;

osEventFlagsId_t 
	flagForward, flagBackward, flagRight, flagLeft, flagStop,
	flagRunningSound, flagEndingSound, 
	flagTurnRightForward, flagTurnLeftForward, flagTurnRightBackWard, flagTurnLeftBackward,
	flagFinish, flagAuto
;

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
		turnRightForward(DUTY_CYCLE_TURN);
	}
}

void tTurnRightBackward(void *argument) {

	for (;;) {
		osEventFlagsWait(flagTurnRightBackWard, 0x01, osFlagsWaitAny, osWaitForever);
		turnRightBackward(DUTY_CYCLE_TURN);
	}
}

void tTurnLefttForward(void *argument) {

	for (;;) {
		osEventFlagsWait(flagTurnLeftForward, 0x01, osFlagsWaitAny, osWaitForever);
		turnLeftForward(DUTY_CYCLE_TURN);
	}
}

void tTurnLeftBackward(void *argument) {

	for (;;) {
		osEventFlagsWait(flagTurnLeftBackward, 0x01, osFlagsWaitAny, osWaitForever);
		turnLeftBackward(DUTY_CYCLE_TURN);
	}
}

void tRunningSound(void *argument) {
	
	for (;;) {
		//osEventFlagsWait(flagRunningSound, 0x01, osFlagsWaitAny, osWaitForever);
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
		if (data == FORWARD) { // forward
			osEventFlagsSet(flagForward, 0x01);
		} else if (data == BACKWARD) { // backward
			osEventFlagsSet(flagBackward, 0x01);
		} else if (data == LEFT) { // left
			osEventFlagsSet(flagLeft, 0x01);
		} else if (data == RIGHT) { // right
			osEventFlagsSet(flagRight, 0x01);
		} else if (data == STOP) { // stop
			
			osEventFlagsClear(flagForward, 0x01);
			osEventFlagsClear(flagBackward, 0x01);
			osEventFlagsClear(flagRight, 0x01);
			osEventFlagsClear(flagLeft, 0x01);

			osEventFlagsClear(flagTurnRightForward, 0x01);
			osEventFlagsClear(flagTurnLeftForward, 0x01);
			osEventFlagsClear(flagTurnRightBackWard, 0x01);
			osEventFlagsClear(flagTurnLeftBackward, 0x01);
		
			osEventFlagsSet(flagStop, 0x01);
			
		} else if (data == STOP_FORWARD) {
			osEventFlagsClear(flagForward, 0x01);
		} else if (data == STOP_BACKWARD) {
			osEventFlagsClear(flagBackward, 0x01);
		} else if (data == STOP_LEFT) {
			osEventFlagsClear(flagLeft, 0x01);
		} else if (data == STOP_RIGHT) {
			osEventFlagsClear(flagRight, 0x01);
		} else if (data == RIGHT_FORWARD) {
			osEventFlagsSet(flagTurnRightForward, 0x01);
		} else if (data == LEFT_FORWARD) {
			osEventFlagsSet(flagTurnLeftForward, 0x01);
		} else if (data == RIGHT_BACKWARD) {
			osEventFlagsSet(flagTurnRightBackWard, 0x01);
		} else if (data == LEFT_BACKWARD) {
			osEventFlagsSet(flagTurnLeftBackward, 0x01);
		} else if (data == FINISH) {
			osEventFlagsSet(flagEndingSound, 0x01);
			osEventFlagsClear(flagRunningSound, 0x01);
		}
		

	}
}

int main (void) {
	
  // System Initialization
  SystemCoreClockUpdate();
	MotorsInit();
	initSound();

	offLED();
	
	
  I2C0Init();
  delay(0x8000);
  InitMPU6050();
  initSonar();
	GPIOInitOutput(PORTD, 1);
  
  calibrateMPU6050();
  initYawAngle();
	
	
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

	flagFinish = osEventFlagsNew(NULL);
	flagAuto = osEventFlagsNew(NULL);

  UARTInit(UART2, PIN_PAIR_3, 9600, true);
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(app_main, NULL, NULL);    // Create application main thread
	
	osThreadNew(tForward, NULL, NULL);
	osThreadNew(tRight, NULL, NULL);
	osThreadNew(tBackward, NULL, NULL);
	osThreadNew(tLeft, NULL, NULL);
	osThreadNew(tStop, NULL, NULL);


	
	runLedCycle1Thread();
	runLedFlash500Thread();
	//turnOnLed();
	
	osThreadNew(tRunningSound, NULL, NULL);
	
	osThreadNew(tTurnRightForward, NULL, NULL);
	osThreadNew(tTurnLefttForward, NULL, NULL);
	osThreadNew(tTurnRightBackward, NULL, NULL);
	osThreadNew(tTurnLeftBackward, NULL, NULL);
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
