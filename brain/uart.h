#ifndef _USART_H_
#define _USART_H_


#include "MKL25Z4.h"
#include <stdbool.h>

typedef enum UART_pin_pair {
  PIN_PAIR_1,
  PIN_PAIR_2,
  PIN_PAIR_3
} UART_pin_pair_t;

void UARTInit(UART_Type* UART, UART_pin_pair_t pin_pair, unsigned long baudrate, bool interrupt_enable);
void UARTTransmit(UART_Type* UART, uint8_t data);

// function declaration, implement later
void UART1_IRQHandler(void);
void UART2_IRQHandler(void);

void UARTWriteInt(UART_Type* UART, int num);
void UARTWriteFloat(UART_Type* UART, float num);

#endif
