/*
 * delay.h
 *
 *  Created on: 22 нояб. 2020 г.
 *      Author: arashamf
 */

#ifndef DELAY_H_
#define DELAY_H_
#include "stm32f10x.h"

void UART2_PutString(char *str);
void ini_TIMER6 ();
void delay_us(uint32_t );
void delay_ms(uint32_t );

RCC_ClocksTypeDef RCC_Clocks;  //объявим структуру

#endif /* DELAY_H_ */
