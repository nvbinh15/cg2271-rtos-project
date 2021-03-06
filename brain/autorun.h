#ifndef _AUTO_RUN_
#define _AUTO_RUN_

#include "MKL25Z4.h"

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

#define ANGLE_CLOSE_ENOUGH 0.5f
#define DIST_CLOSE_ENOUGH  0.5f
//#define MOVE_TIME          800  // for hexagon
//#define STOP_DISTANCE       30  // for hexagon
#define MOVE_TIME            690
#define STOP_DISTANCE        35
#define TURN_SPEED_BASE      27



typedef enum autorun_states {
  WAIT,
  START,
  TURN, 
  END,
  STOPA
} autorun_states_t;

typedef enum turning_states {
  TURN_WAIT,
  TURN_FIRST,
  TURN_SECOND,
  TURN_THIRD,
  TURN_FOURTH,
  TURN_FIFTH,
  TURN_SIXTH,
  TURN_SEVENTH,
  TURN_END
} turning_states_t;

extern autorun_states_t curr_state;
extern turning_states_t turning_curr_state;
extern volatile osEventFlagsId_t flagAutoRun;


void tAutoRun(void *argument);
void tDebugState(void *argument);

#endif
