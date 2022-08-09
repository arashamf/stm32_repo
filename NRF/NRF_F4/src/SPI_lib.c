/*
 * SPI.c
 *
 *  Created on: 18 пїЅпїЅпїЅпїЅ. 2020 пїЅ.
 *      Author: ALEXEY
 */

#include "SPI_lib.h"

//-------------------------функция инициализации SPI1-------------------------------//
void SPI1_ini ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //затактируем порт A

	GPIO_InitTypeDef 	GPIO_ini_SPI1;
	GPIO_ini_SPI1.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7; //выберем пин 5, 6, 7 порта A для настройки
	GPIO_ini_SPI1.GPIO_Mode = GPIO_Mode_AF; //порт А пин 5, 6, 7 на альт. выход (PA5-SCK, PA7-MOSI)
	GPIO_ini_SPI1.GPIO_Speed = GPIO_Medium_Speed; //скорость порта средняя
	GPIO_ini_SPI1.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_ini_SPI1.GPIO_PuPd = GPIO_PuPd_NOPULL; //вход без подтяжки

	GPIO_Init (GPIOA, &GPIO_ini_SPI1);

	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource5), GPIO_AF_SPI1); //SPI1_SCK
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource6), GPIO_AF_SPI1); //SPI1_MISO
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource7), GPIO_AF_SPI1); //SPI1_MOSI

	SET_BIT (GPIOA->MODER, GPIO_MODER_MODER4_0); // PA4 (CS) на выход

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); // запускаем тактовый генератор SPI1

	SPI_InitTypeDef		SPI1_Init;
	SPI1_Init.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //полнодуплексный режим и на приём и на передачу
	SPI1_Init.SPI_Mode = SPI_Mode_Master; //мк в режиме мастер
	SPI1_Init.SPI_DataSize = SPI_DataSize_16b; //16 бит данные
	SPI1_Init.SPI_CPOL = SPI_CPOL_Low; //если бит CPOL сброшен, вывод SCK имеет состояние низкого уровня при простое
	SPI1_Init.SPI_CPHA = SPI_CPHA_1Edge; //если бит CPHA сброшен , то первый край на выводе SCK (нарастающий фронт, если бит CPOL сброшен, спадающий фронт, если бит CPOL установлен) является стробом захвата MSBit.
	SPI1_Init.SPI_NSS = SPI_NSS_Soft;  // программное управление выводом NSS
	SPI1_Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //зададим делитель частоты шины (F_SPI2=F_APB1/Prescaler=6/2=3MHz)
	SPI1_Init.SPI_FirstBit = SPI_FirstBit_MSB; // первый идет старший бит
	SPI1_Init.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI1_Init);

	SPI_Cmd(SPI1, ENABLE); //включим SPI1

	//	NVIC_EnableIRQ(SPI1_IRQn); //включим прерывания от SPI1 в контроллере прерываний IRQ
}

//-------------------------функция инициализации SPI2-------------------------------//
void SPI2_ini ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB, ENABLE); //затактируем порт B

	GPIO_InitTypeDef 	GPIO_ini_SPI2;
	GPIO_ini_SPI2.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15; //выберем пин 13, 15 порта B для настройки
	GPIO_ini_SPI2.GPIO_Mode = GPIO_Mode_AF; //порт А пин 13, 15 на альт. выход
	GPIO_ini_SPI2.GPIO_Speed = GPIO_Medium_Speed; //скорость порта
	GPIO_ini_SPI2.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_ini_SPI2.GPIO_PuPd = GPIO_PuPd_NOPULL; //вход без подтяжки

	GPIO_Init (GPIOB, &GPIO_ini_SPI2);

	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource13), GPIO_AF_SPI2); //SPI2_SCK
	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource14), GPIO_AF_SPI2); //SPI2_MISO
	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource15), GPIO_AF_SPI2); //SPI2_MOSI

	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //затактируем порт A
	SET_BIT (GPIOA->MODER, GPIO_MODER_MODER4_0); // PA4 (CS) на выход

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); // запускаем тактовый генератор SPI1

	SPI_InitTypeDef		SPI2_Init;
	SPI2_Init.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //полнодуплексный режим и на приём и на передачу
	SPI2_Init.SPI_Mode = SPI_Mode_Master; //мк в режиме мастер
	SPI2_Init.SPI_DataSize = SPI_DataSize_16b; //16 бит данные
	SPI2_Init.SPI_CPOL = SPI_CPOL_Low; //если бит CPOL сброшен, вывод SCK имеет состояние низкого уровня при простое
	SPI2_Init.SPI_CPHA = SPI_CPHA_1Edge; //если бит CPHA сброшен , то первый край на выводе SCK (нарастающий фронт, если бит CPOL сброшен, спадающий фронт, если бит CPOL установлен) является стробом захвата MSBit.
	SPI2_Init.SPI_NSS = SPI_NSS_Soft;  // программное управление выводом NSS
	SPI2_Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //зададим делитель частоты шины (F_SPI2=F_APB1/Prescaler=6/2=3MHz)
	SPI2_Init.SPI_FirstBit = SPI_FirstBit_MSB; // первый идет старший бит
	SPI2_Init.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI2_Init);

	SPI_Cmd(SPI2, ENABLE); //включим SPI1

	//	NVIC_EnableIRQ(SPI2_IRQn); //включим прерывания от SPI2 в контроллере прерываний IRQ
}

//-----------------------------ф-я чтения байта по SPI2---------------------------//
uint8_t SPI2_read_byte (uint8_t adress, uint8_t cmd_read)
{
	uint8_t SPI2_data= 0;
	CS_ON; //включим вывод CS
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};  //ожидаем пока буфер передачи не пуст
	*(uint8_t*)&SPI2->DR = (adress + cmd_read); //записываем адрес регистра и бит чтения
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {}; //ожидаем пока буфер передачи не пуст
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {}; //ожидаем пока буфер приёма пуст
	(void) SPI2->DR; //считываем регистр ненужных данных
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {}; //ожидаем пока буфер передачи не пуст
	*(uint8_t*)&SPI2->DR = 0x00; //посылаем нули
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {}; //ожидаем пока буфер передачи не пуст
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {}; //ожидаем пока буфер приёма пуст
	SPI2_data = *(uint8_t*)&SPI2->DR; //получаем полезные данные
	CS_OFF; //отключим вывод CS
	return SPI2_data;
}

//-----------------------------ф-я записи байта по SPI2---------------------------//
void SPI2_write_byte (uint8_t adress, uint8_t data, uint8_t cmd_write)
{
	CS_ON; //включим вывод CS
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};
	*(uint8_t*)&SPI2->DR = (adress + cmd_write); //записываем адрес регистра и бит записи
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
	CS_OFF; //отключим вывод CS
}

//-----------------------------ф-я чтения данных массива по SPI2---------------------------//
void SPI2_read_array(uint8_t adress, uint8_t cmd_read, char *data, uint8_t data_len)  //data_len - количество байт необходимых получить
{
	for(uint8_t i = 0x0; i <= data_len; i++)
		{
		if(i == data_len)
			{
			CS_OFF; //отключим вывод CS;
			break;
			}
		*data++ = SPI2_read_byte (adress+i, cmd_read);
		}
}

//-----------------------------ф-я чтения байта по SPI1---------------------------//
uint8_t SPI1_read_byte (uint8_t adress, uint8_t cmd_read)
{
	uint8_t SPI1_data= 0;
	CS_ON; //включим вывод CS
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {};  //ожидаем пока буфер передачи не пуст
	*(uint8_t*)&SPI1->DR = (adress + cmd_read); //записываем адрес регистра и бит чтения
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {}; //ожидаем пока буфер передачи не пуст
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {}; //ожидаем пока буфер приёма пуст
	(void) SPI1->DR; //считываем регистр ненужных данных
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {}; //ожидаем пока буфер передачи не пуст
	*(uint8_t*)&SPI1->DR = 0x00; //посылаем нули
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {}; //ожидаем пока буфер передачи не пуст
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {}; //ожидаем пока буфер приёма пуст
	SPI1_data = *(uint8_t*)&SPI1->DR; //получаем полезные данные
	CS_OFF; //отключим вывод CS
	return SPI1_data;
}

//-----------------------------ф-я записи байта по SPI1---------------------------//
void SPI1_write_byte (uint8_t adress, uint8_t data, uint8_t cmd_write)
{
	CS_ON; //включим вывод CS
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {};
	*(uint8_t*)&SPI1->DR = (adress + cmd_write); //записываем адрес регистра и бит записи
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {};
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {};
	(void) SPI1->DR;  //считываем регистр ненужных данных
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {};
	*(uint8_t*)&SPI1->DR = data; //записываем полезные данные
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {};
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {};
	(void) SPI1->DR; //считываем регистр ненужных данных
	CS_OFF; //отключим вывод CS
}

//-----------------------------ф-я чтения данных массива по SPI1---------------------------//
void SPI1_read_array(uint8_t adress, uint8_t *data, uint8_t data_len, uint8_t cmd_read)  //data_len - количество байт необходимых получить
{
	for(uint8_t i = 0x0; i <= data_len; i++)
		{
		if(i == data_len)
			{
			CS_OFF; //отключим вывод CS;
			break;
			}
		*data++ = SPI1_read_byte (adress+i, cmd_read);
		}
}

//-----------------------------ф-я записи  массива данных по SPI1---------------------------//
void SPI1_write_array(uint8_t adress, uint8_t *data, uint8_t data_len, uint8_t cmd_read)  //data_len - количество байт необходимых получить
{
	for(uint8_t i = 0x0; i <= data_len; i++)
		{
		if(i == data_len)
			{
			CS_OFF; //отключим вывод CS;
			break;
			}
		SPI1_write_byte (adress+i, data[i], cmd_read);
		}
}
