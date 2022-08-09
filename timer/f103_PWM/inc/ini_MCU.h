/*
 * ini_MCU.h
 *
 *  Created on: 11 апр. 2020 г.
 *      Author: arashamf
 */

#ifndef INI_MCU_H_
#define INI_MCU_H_
#include "stm32f10x.h"

#define BAUDRATE 57600
#define PERIOD 100

void LEDS_ini ();
void UART2_ini ();
uint8_t PLL_ini (uint8_t , uint8_t);
void PWM_timer3_ini ();
void timer4_ini ();

#endif /* INI_MCU_H_ */
