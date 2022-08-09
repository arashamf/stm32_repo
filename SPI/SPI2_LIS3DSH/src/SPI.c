/*
 * SPI.c
 *
 *  Created on: 18 февр. 2020 г.
 *      Author: ALEXEY
 */
#include "stm32f4xx.h"
#include "SPI.h"
#include "delay_lib.h"

//-------------------------функция инициализации SPI2-------------------------------//
void SPI2_ini ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB, ENABLE); //затактируем порт B

	GPIO_InitTypeDef 	GPIO_ini_SPI2;
	GPIO_ini_SPI2.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |  GPIO_Pin_15; //выберем пин 12-15 порта B для настройки
	GPIO_ini_SPI2.GPIO_Mode = GPIO_Mode_AF; //порт А пин 12-15 на альт. выход
	GPIO_ini_SPI2.GPIO_Speed = GPIO_High_Speed; //скорость порта
	GPIO_ini_SPI2.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_ini_SPI2.GPIO_PuPd = GPIO_PuPd_NOPULL; //вход без подтяжки

	GPIO_Init (GPIOB, &GPIO_ini_SPI2);

	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource12), GPIO_AF_SPI2); //PB12 - SPI2_NSS
	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource13), GPIO_AF_SPI2); //PB13 - SPI2_SCK
	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource14), GPIO_AF_SPI2); //PB14 - SPI2_MISO
	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource15), GPIO_AF_SPI2); //PB15 - SPI2_MOSI

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); // запускаем тактовый генератор SPI2

	SPI_InitTypeDef		SPI2_Init_user;
	SPI2_Init_user.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //по двум линиям в полнодуплексном режиме
	SPI2_Init_user.SPI_Mode = SPI_Mode_Master; //мк в режиме мастер
	SPI2_Init_user.SPI_DataSize = SPI_DataSize_8b; //8 бит данные
//	SPI2_Init_user.SPI_CPOL = SPI_CPOL_High; //Если бит CPOL установлен, вывод SCK имеет состояние высокого уровня при простое
	SPI2_Init_user.SPI_CPOL = SPI_CPOL_Low;
	SPI2_Init_user.SPI_CPHA = SPI_CPHA_1Edge;
//	SPI2_Init_user.SPI_CPHA = SPI_CPHA_2Edge; //Если установлен бит CPHA , второй край на выводе SCK (спадающий фрон, если бит CPOL сброшен, восходящий край, если бит CPOL установлен) является стробом захвата MSBit.
	SPI2_Init_user.SPI_NSS = SPI_NSS_Soft;  // программное управление выводом NSS
	SPI2_Init_user.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; //зададим делитель частоты шины
	SPI2_Init_user.SPI_FirstBit = SPI_FirstBit_MSB; // первый идет старший бит
	SPI2_Init_user.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2, &SPI2_Init_user);

	SPI_SSOutputCmd(SPI2, DISABLE);
	SPI_Cmd(SPI2, ENABLE); //включим SPI1
}

//-----------------------------ф-я чтения байта по SPI2---------------------------//
uint8_t SPI2_read_byte (uint8_t adress)
{
	uint8_t SPI2_data= 0;
	SPI_SSOutputCmd(SPI2, ENABLE); //включим вывод CS
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};  //ожидаем пока буфер передачи не пуст
	*(uint8_t*)&SPI2->DR = (adress + LIS3DSH_Cmd_Read); //записываем адрес регистра и бит чтения
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {}; //ожидаем пока буфер передачи не пуст
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {}; //ожидаем пока буфер приёма пуст
	(void) SPI2->DR; //считываем регистр ненужных данных
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {}; //ожидаем пока буфер передачи не пуст
	*(uint8_t*)&SPI2->DR = 0x00; //посылаем нули
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {}; //ожидаем пока буфер передачи не пуст
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {}; //ожидаем пока буфер приёма пуст
	SPI2_data = *(uint8_t*)&SPI2->DR; //получаем полезные данные
	SPI_SSOutputCmd(SPI2, DISABLE); //отключим вывод CS
	return SPI2_data;
}

//-----------------------------ф-я записи байта по SPI2---------------------------//
void SPI2_write_byte (uint8_t adress, uint8_t data)
{
	SPI_SSOutputCmd(SPI2, ENABLE); //включим вывод CS
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};
	*(uint8_t*)&SPI2->DR = (adress + LIS3DSH_Cmd_Write); //записываем адрес регистра и бит записи
	//SPI2->DR = (adress + LIS3DSH_Cmd_Write);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {};
	(void) SPI2->DR;  //считываем регистр ненужных данных
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};
	*(uint8_t*)&SPI2->DR = data; //записываем полезные данные
	//SPI2->DR = data;
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {};
	(void) SPI2->DR; //считываем регистр ненужных данных
	SPI_SSOutputCmd(SPI2, DISABLE); //отключим вывод CS
}

//-----------------------------ф-я чтения данных массива по SPI2---------------------------//
void SPI2_read_array(uint8_t adress, char *data, uint8_t data_len)  //data_len - количество байт необходимых получить
{
	for(uint8_t i = 0x0; i <= data_len; i++)
		{
		if(i == data_len)
			{
			SPI_SSOutputCmd(SPI2, DISABLE); //отключим вывод CS;
			break;
			}
		*data++ = SPI2_read_byte (adress+i);
		}
}

//-------------------------функция инициализации SPI1-------------------------------//
void SPI1_ini ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //затактируем порт А

	SPI_I2S_DeInit (SPI1);

	GPIO_InitTypeDef 	GPIO_ini_SPI1;
	GPIO_ini_SPI1.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; //выберем пин 5-7 порта А для настройки
	GPIO_ini_SPI1.GPIO_Mode = GPIO_Mode_AF; //порт А пин 5-7 на альт. выход
	GPIO_ini_SPI1.GPIO_Speed = GPIO_Fast_Speed; //скорость порта
	GPIO_ini_SPI1.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_ini_SPI1.GPIO_PuPd = GPIO_PuPd_NOPULL; //вход без подтяжки

	GPIO_Init (GPIOA, &GPIO_ini_SPI1);

	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource4), GPIO_AF_SPI1);
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource5), GPIO_AF_SPI1);
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource6), GPIO_AF_SPI1);
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource7), GPIO_AF_SPI1);


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); // запускаем тактовый генератор SPI1

	SPI_InitTypeDef		SPI_Init_user;
	SPI_Init_user.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //по двум линиям в полнодуплексном режиме
	SPI_Init_user.SPI_Mode = SPI_Mode_Master; //мк в режиме мастер
	SPI_Init_user.SPI_DataSize = SPI_DataSize_8b; //8 бит данные
	SPI_Init_user.SPI_CPOL = SPI_CPOL_High; //Если бит CPOL установлен, вывод SCK имеет состояние высокого уровня при простое
//	SPI_Init_user.SPI_CPOL = SPI_CPOL_Low;
//	SPI_Init_user.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_Init_user.SPI_CPHA = SPI_CPHA_2Edge; //Если установлен бит CPHA , второй край на выводе SCK (спадающий фрон, если бит CPOL сброшен, восходящий край, если бит CPOL установлен) является стробом захвата MSBit.
	SPI_Init_user.SPI_NSS = SPI_NSS_Soft;  // программное управление выводом NSS
	SPI_Init_user.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; //зададим делитель частоты шины
	SPI_Init_user.SPI_FirstBit = SPI_FirstBit_MSB; // первый идет старший бит
	SPI_Init_user.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_Init_user); //Инициализирует SPI в соответствии с указанными параметрами в структуре SPI_Init
	SPI_SSOutputCmd(SPI2, DISABLE); //отключим вывод CS

	SPI_Cmd(SPI1, ENABLE);

//	NVIC_EnableIRQ(SPI1_IRQn);


}
