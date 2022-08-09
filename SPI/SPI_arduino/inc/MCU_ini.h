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

#define LED3_Pin GPIO_Pin_12
#define LED4_Pin GPIO_Pin_13
#define LED5_Pin GPIO_Pin_14
#define LED6_Pin GPIO_Pin_15
#define LED_Port GPIOD

#endif
