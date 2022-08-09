/*
 * SPI.c
 *
 *  Created on: 18 РїС—Р…РїС—Р…РїС—Р…РїС—Р…. 2020 РїС—Р….
 *      Author: ALEXEY
 */


#include "SPI_lib.h"
#include "delay_lib.h"
//-------------------------функция инициализации SPI1-------------------------------//
void SPI1_ini ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //затактируем порт А

	SPI_I2S_DeInit (SPI1);

	GPIO_InitTypeDef 	GPIO_ini_SPI1;
	GPIO_ini_SPI1.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; //выберем пин 5-7 порта А для настройки
	GPIO_ini_SPI1.GPIO_Mode = GPIO_Mode_AF; //порт А пин 5-7 на альт. выход
	GPIO_ini_SPI1.GPIO_Speed = GPIO_Low_Speed; //скорость порта
	GPIO_ini_SPI1.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_ini_SPI1.GPIO_PuPd = GPIO_PuPd_NOPULL; //вход без подтяжки

	GPIO_Init (GPIOA, &GPIO_ini_SPI1);

	//GPIO_PinAFConfig (GPIOA, (GPIO_PinSource4), GPIO_AF_SPI1); //PA4 - SPI1_NSS
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource5), GPIO_AF_SPI1); //PA5 - SPI1_SCK
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource6), GPIO_AF_SPI1); //PA6 - SPI1_MISO
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource7), GPIO_AF_SPI1);//PA7 - SPI1_MOSI

	SET_BIT (GPIOA->MODER, GPIO_MODER_MODER4_0); // PA4 (CS) на выход

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); // запускаем тактовый генератор SPI1

	SPI_InitTypeDef		SPI1_Init;
	SPI1_Init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //полнодуплексный режим и на приём и на передачу
	SPI1_Init.SPI_Mode = SPI_Mode_Master; //мк в режиме мастер
	SPI1_Init.SPI_DataSize = SPI_DataSize_8b; //8 бит данные
	SPI1_Init.SPI_CPOL = SPI_CPOL_Low; //если бит CPOL сброшен, вывод SCK имеет состояние низкого уровня при простое
	SPI1_Init.SPI_CPHA = SPI_CPHA_1Edge; //если бит CPHA сброшен , то первый край на выводе SCK (нарастающий фронт, если бит CPOL сброшен, спадающий фронт, если бит CPOL установлен) является стробом захвата MSBit.
	SPI1_Init.SPI_NSS = SPI_NSS_Soft;  // программное управление выводом NSS
	SPI1_Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; //зададим делитель частоты шины
	SPI1_Init.SPI_FirstBit = SPI_FirstBit_MSB; // первый идет старший бит
	SPI1_Init.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI1_Init); //Инициализирует SPI в соответствии с параметрами в структуре SPI2_Init

	SPI_Cmd(SPI1, ENABLE);

//	NVIC_EnableIRQ(SPI1_IRQn);
}

//-------------------------функция инициализации SPI2-------------------------------//
void SPI2_ini ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB, ENABLE); //затактируем порт B

	GPIO_InitTypeDef 	GPIO_ini_SPI2;
	GPIO_ini_SPI2.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; //выберем пин 13, 14, 15 порта B для настройки
	GPIO_ini_SPI2.GPIO_Mode = GPIO_Mode_AF; //порт А пин 13, 14, 15 на альт. выход
	GPIO_ini_SPI2.GPIO_Speed = GPIO_Medium_Speed; //скорость порта
	GPIO_ini_SPI2.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_ini_SPI2.GPIO_PuPd = GPIO_PuPd_NOPULL; //вход без подтяжки

	GPIO_Init (GPIOB, &GPIO_ini_SPI2);

//	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource12), GPIO_AF_SPI2); //SPI2_NSS
	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource13), GPIO_AF_SPI2); //SPI2_SCK
	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource14), GPIO_AF_SPI2); //SPI2_MISO
	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource15), GPIO_AF_SPI2); //SPI2_MOSI

	SET_BIT (GPIOB->MODER, GPIO_MODER_MODER12_0); // PB12 (CS) на выход

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); // запускаем тактовый генератор SPI1

	SPI_InitTypeDef		SPI2_Init;
	SPI2_Init.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //полнодуплексный режим и на приём и на передачу
//	SPI2_Init.SPI_Direction = SPI_Direction_1Line_Tx; //по одной линии только на передачу
	SPI2_Init.SPI_Mode = SPI_Mode_Master; //мк в режиме мастер
	SPI2_Init.SPI_DataSize = SPI_DataSize_8b; //8 бит данные
	SPI2_Init.SPI_CPOL = SPI_CPOL_Low; //если бит CPOL сброшен, вывод SCK имеет состояние низкого уровня при простое
	SPI2_Init.SPI_CPHA = SPI_CPHA_1Edge; //если бит CPHA сброшен , то первый край на выводе SCK (нарастающий фронт, если бит CPOL сброшен, спадающий фронт, если бит CPOL установлен) является стробом захвата MSBit.
	SPI2_Init.SPI_NSS = SPI_NSS_Soft;  // программное управление выводом NSS
	SPI2_Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; //зададим делитель частоты шины (F_SPI2=F_APB1/Prescaler)
	SPI2_Init.SPI_FirstBit = SPI_FirstBit_MSB; // первый идет старший бит
	SPI2_Init.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI2_Init);

	SPI_Cmd(SPI2, ENABLE); //включим SPI2

	//	NVIC_EnableIRQ(SPI2_IRQn); //включим прерывания от SPI2 в контроллере прерываний IRQ
}

//-----------------------------ф-я чтения байта по SPI2---------------------------//
uint8_t SPI2_read_byte (uint8_t adress)
{
	uint8_t SPI2_data= 0;
	CS_ON; //включим вывод CS
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};  //ожидаем пока буфер передачи не пуст
	*(uint8_t*)&SPI2->DR = (adress + Cmd_Read); //записываем адрес регистра и бит чтения
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
void SPI2_write_byte (uint8_t adress, uint8_t data)
{
	CS_ON; //включим вывод CS
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};
	*(uint8_t*)&SPI2->DR = (adress + Cmd_Write); //записываем адрес регистра и бит записи
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {};
	(void) SPI2->DR;  //считываем регистр ненужных данных
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};
	*(uint8_t*)&SPI2->DR = data; //записываем полезные данные
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {};
	(void) SPI2->DR; //считываем регистр ненужных данных
	CS_OFF; //отключим вывод CS
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


//-----------------------------ф-я чтения данных массива по SPI2 из ARDUINO---------------------------//
void SPI2_read_array_ARDUINO(uint8_t point, char *data, uint8_t data_len)  //data_len - количество байт необходимых получить
{
	CS_ON;
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};  //ожидаем пока буфер передачи не опустеет
	SPI_I2S_SendData (SPI2, point);
	for(uint8_t count = 0; count <= data_len; count++)
		{
		if(count == data_len)
			{
			CS_OFF; //отключим вывод CS;
			break;
			}
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {}; //ожидаем пока буфер передачи не опустеет
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {}; //ожидаем пока буфер приёма получит данные
		*data++ = (uint8_t) SPI_I2S_ReceiveData (SPI2);
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};  //ожидаем пока буфер передачи не опустеет
		SPI_I2S_SendData (SPI2, 0x0);
		}
	delay_us (10);
	CS_OFF;
}
