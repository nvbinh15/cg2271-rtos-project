#ifndef _COMMON_
#define _COMMON_

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "MKL25Z4.h"

#define DUTY_CYCLE_TURN	20

#define FORWARD 0x30
#define BACKWARD 0x31
#define LEFT 0x32
#define RIGHT 0x33
#define STOP 0x34
#define STOP_FORWARD 0x35
#define STOP_BACKWARD 0x36
#define STOP_LEFT 0x37
#define STOP_RIGHT 0x38
#define RIGHT_FORWARD 0x39
#define LEFT_FORWARD 0x40
#define RIGHT_BACKWARD 0x41
#define LEFT_BACKWARD 0x42

#define AUTO 0x50
#define FINISH 0x51

extern volatile int r_sound;

extern osEventFlagsId_t 
	flagForward, flagBackward, flagRight, flagLeft, flagStop,
	flagRunningSound, flagEndingSound, 
	flagTurnRightForward, flagTurnLeftForward, flagTurnRightBackWard, flagTurnLeftBackward,
	flagFinish, flagAuto,
	flagRunningLed, flagStationLed
;


#endif