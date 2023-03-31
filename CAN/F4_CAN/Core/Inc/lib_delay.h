
#ifndef __LIB_DELAY_H__
#define __LIB_DELAY_H__

#include "main.h"

#define PCLK1_Clock 42000000

void GetClocksFreq (LL_RCC_ClocksTypeDef *SysClock);
void delay_ms(uint16_t delay);
void delay_us(uint16_t delay);

#endif /* __LIB_DELAY_H__ */

