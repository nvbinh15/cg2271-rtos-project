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
// #include "mpu6050.h"
// #include "sonar.h"
// #include "sensors.h"
// #include "autorun.h"
#include "common.h"

volatile int r_sound;

osEventFlagsId_t 
	flagForward, flagBackward, flagRight, flagLeft, flagStop,
	flagRunningSound, flagEndingSound, 
	flagTurnRightForward, flagTurnLeftForward, flagTurnRightBackWard, flagTurnLeftBackward,
	flagFinish, flagAuto,
	flagRunningLed, flagStationLed
;

void delay (unsigned long delay);
volatile uint8_t data;

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

void tRunningSound(void *argument) {
	
	for (;;) {
		osEventFlagsWait(flagRunningSound, 0x01, osFlagsWaitAny, osWaitForever);
		//play_running_sound();
		
	}
}

void tEndingSound(void *argument) {
	
	for (;;) {
		osEventFlagsWait(flagEndingSound, 0x01, osFlagsWaitAny, osWaitForever);
		play_ending_sound();
	}
}

void clearFlags(void) {
	osEventFlagsClear(flagForward, 0x01);
	osEventFlagsClear(flagBackward, 0x01);
	osEventFlagsClear(flagRight, 0x01);
	osEventFlagsClear(flagLeft, 0x01);
	osEventFlagsClear(flagTurnRightForward, 0x01);
	osEventFlagsClear(flagTurnLeftForward, 0x01);
	osEventFlagsClear(flagTurnRightBackWard, 0x01);
	osEventFlagsClear(flagTurnLeftBackward, 0x01);
}

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {
 
  // ...
	osEventFlagsSet(flagRunningSound, 0x01);
	r_sound = 0;
  for (;;) {
		
		
		if (data == FORWARD) {

			forward();
		} else if (data == BACKWARD) {

			backward();
		} else if (data == LEFT) { 

			left();
		} else if (data == RIGHT) {

			right();
		} else if (data == STOP) {
			clearFlags();
			stop();
		} else if (data == RIGHT_FORWARD) {

			turnRightForward(DUTY_CYCLE_TURN);
		} else if (data == LEFT_FORWARD) {

			turnLeftForward(DUTY_CYCLE_TURN);
		} else if (data == RIGHT_BACKWARD) {
			turnRightBackward(DUTY_CYCLE_TURN);
		} else if (data == LEFT_BACKWARD) {
			turnLeftBackward(DUTY_CYCLE_TURN);
		} else if (data == FINISH) {
			r_sound = 1;
			osEventFlagsClear(flagRunningSound, 0x01);
			osEventFlagsSet(flagEndingSound, 0x01);
		}
		
		if (osEventFlagsGet(flagForward) || osEventFlagsGet(flagBackward) || osEventFlagsGet(flagRight) ||
			osEventFlagsGet(flagLeft) || osEventFlagsGet(flagTurnLeftBackward) || osEventFlagsGet(flagTurnLeftForward) ||
			osEventFlagsGet(flagTurnRightBackWard) || osEventFlagsGet(flagTurnRightForward)) {
				
			osEventFlagsSet(flagRunningLed, 0x01);
			osEventFlagsClear(flagStationLed, 0x01);	

		} else {
			osEventFlagsSet(flagStationLed, 0x01);
			osEventFlagsClear(flagRunningLed, 0x01);		
		}
	}
}

int main (void) {
	
  // System Initialization
  SystemCoreClockUpdate();
	MotorsInit();
	initSound();

	offLED();
	
	
//   I2C0Init();
//   delay(0x8000);
//   InitMPU6050();
//   initSonar();
// 	GPIOInitOutput(PORTD, 1);
  
//   calibrateMPU6050();
//   initYawAngle();
	
	
	flagForward = osEventFlagsNew(NULL);
	flagRight = osEventFlagsNew(NULL);
	flagBackward = osEventFlagsNew(NULL);
	flagLeft = osEventFlagsNew(NULL);
	flagStop = osEventFlagsNew(NULL);

	flagTurnRightForward = osEventFlagsNew(NULL);
	flagTurnLeftForward = osEventFlagsNew(NULL);
	flagTurnRightBackWard = osEventFlagsNew(NULL);
	flagTurnLeftBackward = osEventFlagsNew(NULL);

	flagEndingSound = osEventFlagsNew(NULL);
	flagRunningSound = osEventFlagsNew(NULL);

	flagFinish = osEventFlagsNew(NULL);
	flagAuto = osEventFlagsNew(NULL);
	
	flagStationLed = osEventFlagsNew(NULL);
	flagRunningLed = osEventFlagsNew(NULL);

  UARTInit(UART2, PIN_PAIR_3, 9600, true);
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(app_main, NULL, NULL);    // Create application main thread
	
	runLedCycle1Thread();
	runLedFlash500Thread();
	turnOnLed();
	runLedFlash200Thread();
	
	osThreadNew(tRunningSound, NULL, NULL);
	osThreadNew(tEndingSound, NULL, NULL);
	

  osKernelStart();                      // Start thread execution
  for (;;) {}
}
