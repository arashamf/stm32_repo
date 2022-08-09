#ifndef MCU_ini
#define MCU_ini
#include "stm32f4xx.h"

void LEDS_ini ();
void UART2_ini ();
void UART3_ini ();
uint8_t HSE_ini ();
uint8_t HSI_ini ();
uint8_t PLL_ini (uint8_t , uint8_t , uint32_t , uint32_t, uint32_t );
uint8_t PLL_ini_SPL (uint8_t , uint8_t , uint32_t , uint32_t, uint8_t);
void DAC_ini ();
void ini_button_B1 ();
void init_stop_mode ();

#endif
