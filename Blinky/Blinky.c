/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2015 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "MKL25Z4.h"                    // Device header
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "Board_LED.h"                  // ::Board Support:LED


/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_LED': Sample thread
 *---------------------------------------------------------------------------*/

void Thread_LED (void const *argument);                             // thread function
osThreadId tid_Thread_LED;                                          // thread id
osThreadDef (Thread_LED, osPriorityNormal, 1, 0);                   // thread object

int Init_Thread_LED (void) {

  tid_Thread_LED = osThreadCreate (osThread(Thread_LED), NULL);
  if(!tid_Thread_LED) return(-1);

  return(0);
}

void Thread_LED (void const *argument) {
  uint32_t led_max    = LED_GetCount();
  uint32_t led_num    = 0;


  while (1) {
    LED_On(led_num);                                                // Turn specified LED on
    osSignalWait(0x0001, osWaitForever);
    LED_Off(led_num);                                               // Turn specified LED off
    osSignalWait(0x0001, osWaitForever);

    led_num++;                                                      // Change LED number
    if (led_num >= led_max) {
      led_num = 0;                                                  // Restart with first LED
    }

    osThreadYield();                                                // suspend thread
  }
}



/*----------------------------------------------------------------------------
 * main: blink LED and check button state
 *----------------------------------------------------------------------------*/
int main (void) {

  osKernelInitialize ();                                            // initialize CMSIS-RTOS

  SystemCoreClockUpdate();

  LED_Initialize();                                                 // initalize LEDs

  Init_Thread_LED();                                                // create LED thread

  osKernelStart ();                                                 // start thread execution

  for (;;) {                                                        // main must not be terminated!
    osDelay(500);

    osSignalSet(tid_Thread_LED, 0x0001);
  }
}
