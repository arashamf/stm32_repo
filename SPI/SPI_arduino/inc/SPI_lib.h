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

void SPI1_ini ();
void SPI2_ini ();
uint8_t SPI2_read_byte (uint8_t );
void SPI2_write_byte (uint8_t , uint8_t );
void SPI2_read_array(uint8_t, char *, uint8_t);
void SPI2_read_array_ARDUINO(uint8_t , char *, uint8_t);

#define CS_ON GPIO_ResetBits (GPIOB, GPIO_Pin_12) //активируем чип-селект
#define CS_OFF GPIO_SetBits (GPIOB, GPIO_Pin_12) //выключим чип-селект

#endif /* SPI_H_ */
