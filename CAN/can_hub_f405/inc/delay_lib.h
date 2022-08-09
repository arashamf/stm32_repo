/*
 * delay_lib.h
 *
 *  Created on: 17 февр. 2020 г.
 *      Author: ALEXEY
 */

#ifndef DELAY_LIB_H_
#define DELAY_LIB_H_

#include "stm32f4xx.h"

void ini_TIMER5 ();
void delay_us (uint32_t );
void delay_ms (uint32_t );

RCC_ClocksTypeDef RCC_Clocks;  //объ€вим структуру

#endif /* DELAY_LIB_H_ */
