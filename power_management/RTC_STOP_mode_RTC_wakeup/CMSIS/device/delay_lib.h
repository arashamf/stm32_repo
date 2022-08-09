/*
 * delay_lib.h
 *
 *  Created on: 17 ����. 2020 �.
 *      Author: ALEXEY
 */

#ifndef DELAY_LIB_H_
#define DELAY_LIB_H_
#include "stm32f4xx.h"

void ini_TIMER5 ();
void delay_us(uint32_t );
void delay_ms(uint32_t );
void ini_TIMER4 (uint32_t);

RCC_ClocksTypeDef RCC_Clocks;  //объявим структуру c тактовыми частотами

#endif /* DELAY_LIB_H_ */
