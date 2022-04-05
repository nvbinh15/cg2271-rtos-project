#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "MKL25Z4.h"

typedef enum motor_direction {
  FORWARD,
  REVERSE
} motor_direction_t;

void MotorsInit(void);
uint32_t dutyCycleToPWM(int duty_cycle);
void leftMove(motor_direction_t direction, int value);
void rightMove(motor_direction_t direction, int value);

/**
 * BELOW defintions NEWLY ADDED
 * Remember to remove identical ones in GPIO including motorinit
 */
void forward(void);
void backward(void);
void right(void);
void left(void);
void stop(void);

void turnRightForward(int duty_cycle);
void turnLeftForward(int duty_cycle);
void turnRightBackward(int duty_cycle);
void turnLeftBackward(int duty_cycle);

#endif
