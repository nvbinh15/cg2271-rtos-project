#include "motor.h"

void MotorsInit(void) {
  // enable clock to port B
  SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
  
  // Alternate function 3 on PORTB pin 0, 1, 2, 3
  // Pin B1
  PORTB->PCR[0] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[0] = PORT_PCR_MUX(3);
  // Pin B0
  PORTB->PCR[1] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[1] = PORT_PCR_MUX(3);
  
  PORTB->PCR[2] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[2] = PORT_PCR_MUX(3);
  
  PORTB->PCR[3] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[3] = PORT_PCR_MUX(3);
  
  // enable clock to TPM1 and TPM2
  SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK | SIM_SCGC6_TPM2_MASK;

  // choose default 48mHz clock
  SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
  SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
  
  // choose mod value
  TPM1->MOD = 6000;
  TPM2->MOD = 6000;
  
  // Setup prescaler and counting mode
  TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
  TPM2->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
  
  TPM1->SC |= TPM_SC_CMOD(1) | TPM_SC_PS(4);
  TPM2->SC |= TPM_SC_CMOD(1) | TPM_SC_PS(4);
  
  TPM1->SC &= ~TPM_SC_CPWMS_MASK;
  TPM2->SC &= ~TPM_SC_CPWMS_MASK;
  
  // edge-aligned, clear output on match
  TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
  
  TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
  
  TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
  
  TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM2_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
  
  TPM1->CNT = 0;
  TPM2->CNT = 0;
}

uint32_t dutyCycleToPWM(float duty_cycle) {
  if (duty_cycle > 100) 
    duty_cycle = 100;
  if (duty_cycle < 0) 
    duty_cycle = 0;
  return (uint32_t) (((float) duty_cycle / 100.0f) * (6000 + 1));
}

void rightMove(motor_direction_t direction, float value) {
  uint32_t pwm_value = dutyCycleToPWM(value);
  switch (direction) {
    case REVERSE:
      TPM1_C0V = pwm_value;
      TPM1_C1V = 0;
      break;
    
    case FOWARD:
      TPM1_C0V = 0;
      TPM1_C1V = pwm_value;
      break;
  }
}

void leftMove(motor_direction_t direction, float value) {
  uint32_t pwm_value = dutyCycleToPWM(value);
  switch (direction) {
    case REVERSE:
      TPM2_C0V = pwm_value;
      TPM2_C1V = 0;
      break;
    
    case FOWARD:
      TPM2_C0V = 0;
      TPM2_C1V = pwm_value;
      break;
  }
}

void motorStop(void) {
  TPM1_C0V = 6000;
  TPM1_C1V = 6000;
  TPM2_C0V = 6000;
  TPM2_C1V = 6000;
}

/**
 * BELOW FUNCTIONS NEWLY ADDED
 * Remember to remove identical ones in GPIO
 */

/**
 * Move forward with 100% Duty cycle
 */ 
void forward(void) {
  leftMove(FOWARD, 90);
  rightMove(FOWARD, 100);
}

/**
 * Move backward with 100% Duty cycle
 */
void backward(void) {
  leftMove(REVERSE, 90);
  rightMove(REVERSE, 100);
}

/**
 * Pivot turn right with 100% Duty cycle
 */
void right(void) {
  leftMove(FOWARD, 50);
  rightMove(REVERSE, 50);
}

/**
 * Pivot turn left with 100% Duty cycle
 */
void left(void) {
  leftMove(REVERSE, 50);
  rightMove(FOWARD, 50);
}

/**
 * Duty cycle for TPM1 C0+C1 TPM2 C0+C1 set to 0
 */
void stop(void) {
  motorStop();
}

/**
 * Curve turn RIGHT while moving FORWARD
 * Left wheels default to 100 duty cycle
 * Vary duty_cycle parameter to change turn curvature
 */
void turnRightForward(int duty_cycle) {
  leftMove(FOWARD, 100);
  rightMove(FOWARD, duty_cycle);
}

/**
 * Curve turn LEFT while moving FORWARD
 * Right wheels default to 100 duty cycle
 * Vary duty_cycle parameter to change turn curvature
 */
void turnLeftForward(int duty_cycle) {
  leftMove(FOWARD, duty_cycle);
  rightMove(FOWARD, 100);
}

/**
 * Curve turn RIGHT while moving BACKWARDS
 * Left wheels default to 100 duty cycle
 * Vary duty_cycle parameter to change turn curvature
 */
void turnRightBackward(int duty_cycle) {
  leftMove(REVERSE, 100);
  rightMove(REVERSE, duty_cycle);  
}

/**
 * Curve turn LEFT while moving BACKWARDS
 * Right wheels default to 100 duty cycle
 * Vary duty_cycle parameter to change turn curvature
 */
void turnLeftBackward(int duty_cycle) {
  leftMove(REVERSE, duty_cycle);
  rightMove(REVERSE, 100);  
}
