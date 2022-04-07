#include "MKL25Z4.h"
#include "mpu6050.h"
#include "sonar.h"
#include "sensors.h"
#include "gpio.h"
#include "clock.h"
#include "uart.h"
#include "motor.h"
#include "autorun.h"
#include "math.h"
#include "common.h"

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

autorun_states_t curr_state = WAIT;
static autorun_states_t next_state;

turning_states_t turning_curr_state = TURN_WAIT;
static turning_states_t next_turning_state;

static uint32_t currTime;
static uint32_t endTime;

static osTimerId_t sonarCallbackId;
volatile osEventFlagsId_t flagAutoRun;

void tAutoRun(void *argument) {
  for (;;) {
    // get flags
    osEventFlagsWait(flagAutoRun, 0x01, osFlagsNoClear, osWaitForever);
    sonarCallbackId = osTimerNew(sonarCallback, osTimerPeriodic, NULL, NULL);
    osTimerStart(sonarCallbackId, 100U);
    
    osDelay(1000);
    
    curr_state = START;
    next_state = START;
    
    while (curr_state != STOPA && osEventFlagsGet(flagAutoRun) == 0x01) {
      // fsm 
      if (curr_state == START) {
        currTime = osKernelGetTickCount();
        while (sonarDistance > STOP_DISTANCE) {
          leftMove(FOWARD, 95 - gyro_yaw_input);
          rightMove(FOWARD, 95 + gyro_yaw_input);
        }
        
        endTime = osKernelGetTickCount() - currTime + 100;    // for ending exit
        
        currTime = osKernelGetTickCount();
        
        while (osKernelGetTickCount() - currTime < 300) {
          motorStop();
        }
        osTimerStop(sonarCallbackId);
        next_state = TURN;
      } 
      else if (curr_state == TURN) {
        turning_curr_state = TURN_FIRST;
        next_turning_state = TURN_FIRST;
        while (turning_curr_state != TURN_END) {
          // fsm
          // left turn
          if (turning_curr_state == TURN_FIRST) {
            //target_heading = angle_yaw - 60;
            angle_yaw = 0;
            // heading correction
            while (fabs(angle_yaw) < 45) {
              float correction = fabs(-45 - angle_yaw) * 0.2;
              leftMove(REVERSE, TURN_SPEED_BASE + correction);
              rightMove(FOWARD, TURN_SPEED_BASE + correction);
            }
            
            // move straight for a few moment
            currTime = osKernelGetTickCount();
            while (osKernelGetTickCount() - currTime < MOVE_TIME) {
              leftMove(FOWARD, 95 - gyro_yaw_input);
              rightMove(FOWARD, 95 + gyro_yaw_input);
            }
						
						currTime = osKernelGetTickCount();
						while (osKernelGetTickCount() - currTime < 150) {
							motorStop();
						}
            
            next_turning_state = TURN_SECOND;     
          }
          
          // right turn
          else if (turning_curr_state == TURN_SECOND) {
            //target_heading = angle_yaw + 60;
            angle_yaw = 0;
            while (fabs(angle_yaw) < 90) {
              float correction = fabs(angle_yaw - 90) * 0.2;
              leftMove(FOWARD, TURN_SPEED_BASE + correction);
              rightMove(REVERSE, TURN_SPEED_BASE + correction);
            }
            
            // move straight for a few moment
            currTime = osKernelGetTickCount();
            while (osKernelGetTickCount() - currTime < MOVE_TIME) {
              leftMove(FOWARD, 95 - gyro_yaw_input);
              rightMove(FOWARD, 95 + gyro_yaw_input);
            }
            
						currTime = osKernelGetTickCount();
						while (osKernelGetTickCount() - currTime < 150) {
							motorStop();
						}
						
            next_turning_state = TURN_THIRD;
          }
          
          // right turn
          else if (turning_curr_state == TURN_THIRD) {
            //target_heading = angle_yaw + 60;
            angle_yaw = 0;
            while (fabs(angle_yaw) < 90) {
              float correction = fabs(angle_yaw - 90) * 0.2;
              leftMove(FOWARD, TURN_SPEED_BASE + correction);
              rightMove(REVERSE, TURN_SPEED_BASE + correction);
            }
            
            // move straight for a few moment
            currTime = osKernelGetTickCount();
            while (osKernelGetTickCount() - currTime < MOVE_TIME) {
              leftMove(FOWARD, 95 - gyro_yaw_input);
              rightMove(FOWARD, 95 + gyro_yaw_input);
            }
            
						currTime = osKernelGetTickCount();
						while (osKernelGetTickCount() - currTime < 150) {
							motorStop();
						}
						
            next_turning_state = TURN_FOURTH;
          }
          
          // right turn
          else if (turning_curr_state == TURN_FOURTH) {
            //target_heading = angle_yaw + 60;
            angle_yaw = 0;
            while (fabs(angle_yaw) < 90) {
              float correction = fabs(angle_yaw - 90) * 0.2;
              leftMove(FOWARD, TURN_SPEED_BASE + correction);
              rightMove(REVERSE, TURN_SPEED_BASE + correction);
            }
            
            // move straight for a few moment
            currTime = osKernelGetTickCount();
            while (osKernelGetTickCount() - currTime < MOVE_TIME) {
              leftMove(FOWARD, 95 - gyro_yaw_input);
              rightMove(FOWARD, 95 + gyro_yaw_input);
            }
						
						currTime = osKernelGetTickCount();
						while (osKernelGetTickCount() - currTime < 150) {
							motorStop();
						}
            
            next_turning_state = TURN_FIFTH;
          }
          
          // left turn
          else if (turning_curr_state == TURN_FIFTH) {
            //target_heading = angle_yaw + 60;
            angle_yaw = 0;
            while (fabs(angle_yaw) < 60) {
              float correction = fabs(-60 - angle_yaw) * 0.2;
              leftMove(REVERSE, TURN_SPEED_BASE + correction);
              rightMove(FOWARD, TURN_SPEED_BASE + correction);
            }
            next_turning_state = TURN_END; // exit the turn loop
          }
  
          turning_curr_state = next_turning_state;
        }
        
        next_state = END;
      }
      
      else if (curr_state == END) {
        currTime = osKernelGetTickCount();
        while (osKernelGetTickCount() - currTime < endTime) {
          leftMove(FOWARD, 95 - gyro_yaw_input);
          rightMove(FOWARD, 95 + gyro_yaw_input);
        }
        next_state = STOPA;
      }
      
      curr_state = next_state;
    }
    currTime = osKernelGetTickCount();
    while (osKernelGetTickCount() - currTime < 100) {
      motorStop();

      osEventFlagsClear(flagAutoRun, 0x01);
			osEventFlagsClear(flagRunning, 0x01);
			osEventFlagsClear(flagRunningSound, 0x01);
			osEventFlagsSet(flagStation, 0x01);
			osEventFlagsSet(flagEndingSound, 0x01);
		
    }
  }
}


void tDebugState(void *argument) {
  for(;;) {
    if (curr_state == WAIT) {
      UARTTransmit(UART2, 'W');
    }
    
    else if (curr_state == START) {
      UARTTransmit(UART2, 'S');
    }
    else if (curr_state == TURN) {
      UARTTransmit(UART2, 'T');
    }
    
    else  {
      UARTTransmit(UART2, 'E');
    }
    
    UARTTransmit(UART2, '\t');
    
    if (turning_curr_state == TURN_WAIT) {
      UARTTransmit(UART2, 'W');
    }
    
    else if (turning_curr_state == TURN_FIRST) {
      UARTTransmit(UART2, '1');
    }
    
    else if (turning_curr_state == TURN_SECOND) {
      UARTTransmit(UART2, '2');
    }
    
    else if (turning_curr_state == TURN_THIRD) {
      UARTTransmit(UART2, '3');
    }
    
    else if (turning_curr_state == TURN_FOURTH) {
      UARTTransmit(UART2, '4');
    }
    
    else if (turning_curr_state == TURN_FIFTH) {
      UARTTransmit(UART2, '5');
    }
    
    else if (turning_curr_state == TURN_SIXTH) {
      UARTTransmit(UART2, '6');
    }
    
    else if (turning_curr_state == TURN_SEVENTH) {
      UARTTransmit(UART2, '7');
    }
    
    else {
      UARTTransmit(UART2, 'E');
    }
    
    UARTTransmit(UART2, '\t');
    UARTWriteFloat(UART2, sonarDistance);
    UARTTransmit(UART2, '\t');
    UARTWriteFloat(UART2, angle_yaw);
    UARTTransmit(UART2, '\n');
    osDelay(200);
  }
}
