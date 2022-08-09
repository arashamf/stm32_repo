#ifndef MCU_ini
#define MCU_ini
#include "stm32f4xx.h"

#define BAUDRATE 57600

void LEDS_ini ();
void UART2_ini ();
void DMA1_ini ();
void RTC_ini ();
uint8_t PLL_ini (uint8_t , uint8_t , uint32_t , uint32_t );

#endif
