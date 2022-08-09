#ifndef MCU_ini
#define MCU_ini
#include "stm32f4xx.h"
void LEDS_ini ();
void UART2_ini ();
void DMA1_ini ();
uint8_t PLL_ini (uint8_t , uint8_t , uint32_t , uint32_t );
#define BAUDRATE 57600

#endif
