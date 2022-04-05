#include "MKL25Z4.h"
#include "uart.h"
#include <stdbool.h>


/**
  * Initialize the UART port with the baud rate.
  * 
  * Since we are not using UART0 for the project, when trying to
  * input to initialize UART0, it will raise an error in the calling function.
  * 
  * @PARAM[IN] UART: UART port to select (UART1 or UART2)
  * @PARAM[IN] PIN_PAIR: Pin Pair to select for UART2, since there are 3 pairs
  * of pins that can perform UART2 functions. For UART1, this doesn't matter.
  * 
  * PIN_PAIR_1: PTD2 and PTD3
  * PIN_PAIR_2: PTD4 and PTD5
  * PIN_PAIR_3: PTE22 and PTE23
  *
  * @PARAM[IN] baudrate: baudrate to operate the uart port
  * @PARAM[IN] interrupt_enable: true if using interrupt, false if not
  */

void UARTInit(UART_Type* UART, UART_pin_pair_t pin_pair, unsigned long baudrate, bool interrupt_enable) {
  uint32_t divisor, bus_clock;
  
  bus_clock = DEFAULT_SYSTEM_CLOCK / 2;
  divisor = bus_clock / (baudrate * 16);
  
  // PTC3 and PTC4
  if (UART == UART1) {
    SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    
    PORTC->PCR[3] &= ~(PORT_PCR_MUX_MASK);
    PORTC->PCR[4] &= ~(PORT_PCR_MUX_MASK);
    
    PORTC->PCR[3] |= PORT_PCR_MUX(3);
    PORTC->PCR[4] |= PORT_PCR_MUX(3);
    
    UART1->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));
    
    UART1->BDH = UART_BDH_SBR(divisor >> 8);
    UART1->BDL = UART_BDH_SBR(divisor);
    
    UART1->C1 = 0;
    UART1->S2 = 0;
    UART1->C3 = 0;
    
    if (interrupt_enable == 0) {
      UART1->C2 |= (UART_C2_TE_MASK) | (UART_C2_RE_MASK);
    }
    else {
      NVIC_SetPriority(UART1_IRQn, 1);
      NVIC_ClearPendingIRQ(UART1_IRQn);
      NVIC_EnableIRQ(UART1_IRQn);
      
      UART1->C2 |= UART_C2_RIE_MASK | UART_C2_TE_MASK | UART_C2_RE_MASK;
    }
  }
  
  else {
    SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
    
    // Pin D2 and D3 (PTD2 and PTD3)
    if (pin_pair == PIN_PAIR_1) {
      SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
      
      PORTD->PCR[2] &= ~(PORT_PCR_MUX_MASK);
      PORTD->PCR[3] &= ~(PORT_PCR_MUX_MASK);
      
      PORTD->PCR[2] |= PORT_PCR_MUX(3);
      PORTD->PCR[3] |= PORT_PCR_MUX(3);
    }
    
    // Pin D4 and D5 (PTD4 and PTD5)
    else if (pin_pair == PIN_PAIR_2) {
      SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
      
      PORTD->PCR[4] &= ~(PORT_PCR_MUX_MASK);
      PORTD->PCR[5] &= ~(PORT_PCR_MUX_MASK);
      
      PORTD->PCR[4] |= PORT_PCR_MUX(3);
      PORTD->PCR[5] |= PORT_PCR_MUX(3);
    }
    
    // PTE22 and PTE23
    else {
      SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
      
      PORTE->PCR[22] &= ~(PORT_PCR_MUX_MASK);
      PORTE->PCR[23] &= ~(PORT_PCR_MUX_MASK);
      
      PORTE->PCR[22] |= PORT_PCR_MUX(4);
      PORTE->PCR[23] |= PORT_PCR_MUX(4);
    }
    
    UART2->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));
     
    UART2->BDH = UART_BDH_SBR(divisor >> 8);
    UART2->BDL = UART_BDL_SBR(divisor);
  
    UART2->C1 = 0;
    UART2->S2 = 0;
    UART2->C3 = 0;
    
    if (interrupt_enable == 0) {
      UART2->C2 |= UART_C2_RE_MASK | UART_C2_TE_MASK;
    }
    
    else {
      NVIC_SetPriority(UART2_IRQn, 1);
      NVIC_ClearPendingIRQ(UART2_IRQn);
      NVIC_EnableIRQ(UART2_IRQn);
      
      UART2->C2 |= UART_C2_RIE_MASK | UART_C2_TE_MASK | UART_C2_RE_MASK;
    }
  }
}

void UARTTransmit(UART_Type* UART, uint8_t data) {
  while(!(UART->S1 & UART_S1_TDRE_MASK));
  UART->D = data;
}

void UARTWriteInt(UART_Type* UART, int num) {
  if (num < 0) {
      UARTTransmit(UART, '-');
  }
  
  //UARTTransmit(UART, abs(num)/1000 + '0');
  UARTTransmit(UART, (abs(num)/100)%10 + '0');
  UARTTransmit(UART, (abs(num)/10)%10 + '0');
  UARTTransmit(UART, (abs(num))%10 + '0');
}

void UARTWriteFloat(UART_Type* UART, float num) {
  if (num < 0) {
      UARTTransmit(UART, '-');
  }
  
  num = num * 10;
  UARTTransmit(UART, abs(num)/1000 + '0');
  UARTTransmit(UART, (abs(num)/100)%10 + '0');
  UARTTransmit(UART, (abs(num)/10)%10 + '0');
  UARTTransmit(UART, '.');
  UARTTransmit(UART, (abs(num))%10 + '0');
}
