/*
 * SPI.h
 *
 *  Created on: 18 февр. 2020 г.
 *      Author: ALEXEY
 */

#ifndef SPI_H_
#define SPI_H_

#include "stm32f4xx.h"

#define Cmd_Read 0x80 // команда чтения
#define Cmd_Write 0x00 // команда записи

uint8_t SPI2_read_byte (uint8_t );
void SPI2_write_byte (uint8_t , uint8_t );
void SPI2_read_array(uint8_t, char *, uint8_t);
void SPI2_ini ();
void SPI1_ini ();

#define CS_ON GPIO_ResetBits (GPIOA, GPIO_Pin_4) //активируем чип-селект
#define CS_OFF GPIO_SetBits (GPIOA, GPIO_Pin_4) //выключим чип-селект

#endif /* SPI_H_ */
