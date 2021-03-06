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
#include "common.h"

int running_song[] = {
  NOTE_FS5, NOTE_FS5,NOTE_D5, NOTE_B4, REST, NOTE_B4, REST, NOTE_E5, 
  REST, NOTE_E5, REST, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5,
  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, REST, NOTE_D5, REST, NOTE_FS5, 
  REST, NOTE_FS5, REST, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5,
  NOTE_FS5, NOTE_FS5,NOTE_D5, NOTE_B4, REST, NOTE_B4, REST, NOTE_E5, 
  
  REST, NOTE_E5, REST, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5,
  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, REST, NOTE_D5, REST, NOTE_FS5, 
  REST, NOTE_FS5, REST, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5,
  NOTE_FS5, NOTE_FS5,NOTE_D5, NOTE_B4, REST, NOTE_B4, REST, NOTE_E5, 
  REST, NOTE_E5, REST, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5,
  
  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, REST, NOTE_D5, REST, NOTE_FS5, 
  REST, NOTE_FS5, REST, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5
};

osEventFlagsId_t 
	flagRunningSound, flagEndingSound, 
	flagFinish, flagAuto,
	flagRunning, flagStation
;

void delay (unsigned long delay);
volatile uint8_t data;

// sensor related callback (50hz)
static void timerStart(void) {
  osTimerId_t sensorCallbackId;
   sensorCallbackId = osTimerNew(imuCallback, osTimerPeriodic, NULL, NULL);

  if (sensorCallbackId != NULL) {
    osStatus_t timerStatus = osTimerStart(sensorCallbackId, 20U);
    
    if (timerStatus != osOK) {
      GPIOSetOutput(PORTB, 19, LOW);
    }
  }
}

void tTimerStart(void *argument) {
   // auto run
  flagAutoRun = osEventFlagsNew(NULL);
  osEventFlagsClear(flagAutoRun, 0x01);
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

/* IRQ Handler for UART */
void UART2_IRQHandler(void) {
  NVIC_ClearPendingIRQ(UART2_IRQn);
	
  if (UART2->S1 & UART_S1_RDRF_MASK) {
    data = UART2->D;
  }
}

/* Play running sound thread */
void tRunningSound(void *argument) {
	
	for (;;) {
		
		for (int i = 0; i < RUNNING_SOUND_LEN; i++) {
			osEventFlagsWait(flagRunningSound, 0x01, osFlagsNoClear, osWaitForever);
			if (running_song[i] != 0) {
				TPM0->MOD = 375000 / running_song[i];
				TPM0_C3V =  375000 / running_song[i] / 10;
			}
			osDelay(200);
		}
	}
}

/* Play ending sound thread */
void tEndingSound(void *argument) {
	
	for (;;) {
		osEventFlagsWait(flagEndingSound, 0x01, osFlagsWaitAny, osWaitForever);
		play_ending_sound();
	}
}


/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {
 
	osEventFlagsClear(flagRunning, 0x01);
	osEventFlagsSet(flagRunningSound, 0x01);
	osEventFlagsSet(flagStation, 0x01);

  for (;;) {
		
		
		if (data == FORWARD) {
			osEventFlagsSet(flagRunning, 0x01);
			osEventFlagsClear(flagStation, 0x01);
			forward();
		} else if (data == BACKWARD) {
			osEventFlagsSet(flagRunning, 0x01);
			osEventFlagsClear(flagStation, 0x01);
			backward();
		} else if (data == LEFT) { 
			osEventFlagsSet(flagRunning, 0x01);
			osEventFlagsClear(flagStation, 0x01);
			left();
		} else if (data == RIGHT) {
			osEventFlagsSet(flagRunning, 0x01);
			osEventFlagsClear(flagStation, 0x01);
			right();
		} else if (data == STOP) {
			osEventFlagsClear(flagAutoRun, 0x01);
			osEventFlagsSet(flagStation, 0x01);
			osEventFlagsClear(flagRunning, 0x01);
			stop();
		} else if (data == RIGHT_FORWARD) {
			osEventFlagsSet(flagRunning, 0x01);
			osEventFlagsClear(flagStation, 0x01);
			turnRightForward(DUTY_CYCLE_TURN);
		} else if (data == LEFT_FORWARD) {
			osEventFlagsSet(flagRunning, 0x01);
			osEventFlagsClear(flagStation, 0x01);
			turnLeftForward(DUTY_CYCLE_TURN);
		} else if (data == RIGHT_BACKWARD) {
			osEventFlagsSet(flagRunning, 0x01);
			osEventFlagsClear(flagStation, 0x01);
			turnRightBackward(DUTY_CYCLE_TURN);
		} else if (data == LEFT_BACKWARD) {
			osEventFlagsSet(flagRunning, 0x01);
			osEventFlagsClear(flagStation, 0x01);
			turnLeftBackward(DUTY_CYCLE_TURN);
		} else if (data == FINISH) {
			osEventFlagsClear(flagRunningSound, 0x01);
			osEventFlagsSet(flagEndingSound, 0x01);
		} else if (data == SILENT) {
			stop_sound();
			osEventFlagsClear(flagRunningSound, 0x01);
			osEventFlagsClear(flagEndingSound, 0x01);			
		} else if (data == AUTO) {
			osEventFlagsClear(flagEndingSound, 0x01);
			osEventFlagsClear(flagStation, 0x01);
			osEventFlagsSet(flagRunning, 0x01);
			osEventFlagsSet(flagRunningSound, 0x01);			
			osEventFlagsSet(flagAutoRun, 0x01);
			data = 0;
		} else if (data == RUNNING_SOUND) {
			osEventFlagsClear(flagEndingSound, 0x01);
			osEventFlagsSet(flagRunningSound, 0x01);
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
	
	
	flagEndingSound = osEventFlagsNew(NULL);
	flagRunningSound = osEventFlagsNew(NULL);

	flagFinish = osEventFlagsNew(NULL);
	flagAuto = osEventFlagsNew(NULL);
	
	flagStation = osEventFlagsNew(NULL);
	flagRunning = osEventFlagsNew(NULL);
	
	
 
  UARTInit(UART2, PIN_PAIR_3, 9600, true);
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(app_main, NULL, NULL);    // Create application main thread
	 
	osThreadNew(tTimerStart, NULL, NULL);
	osThreadNew(tAutoRun, NULL, NULL);
	
	// Initialize LED threads
	runLedCycle1Thread();
	runLedFlash500Thread();
	turnOnLed();
	runLedFlash200Thread();
	
	osThreadNew(tRunningSound, NULL, NULL);
	osThreadNew(tEndingSound, NULL, NULL);

	// auto run
	osThreadNew(tDebugState, NULL, NULL);
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
