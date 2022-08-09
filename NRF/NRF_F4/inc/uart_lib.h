/*
 * uart_lib.h
 *
 *  Created on: 4 дек. 2020 г.
 *      Author: ALEXEY
 */

#ifndef UART_LIB_H_
#define UART_LIB_H_

#include "stm32f4xx.h"

#define BAUDRATE 57600

void UART2_ini ();
void UART3_ini ();
void UART3_PutString(char *);


#endif /* UART_LIB_H_ */
