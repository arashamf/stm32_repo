/*
 * uart_lib.c
 *
 *  Created on: 4 ���. 2020 �.
 *      Author: ALEXEY
 */

#include "uart_lib.h"

//-------------------------функция инициализации UART2-------------------------------//
void UART2_ini ()

	{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //затактируем порт А

	GPIO_InitTypeDef 	GPIO_Init_UART2;
	GPIO_Init_UART2.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //PA2 - Tx, PA3 - Rx
	GPIO_Init_UART2.GPIO_Mode = GPIO_Mode_AF; //порт А пин 2, 3 на альт. выход
	GPIO_Init_UART2.GPIO_Speed = GPIO_Fast_Speed; //скорость порта высокая
	GPIO_Init_UART2.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_Init_UART2.GPIO_PuPd = GPIO_PuPd_UP; //вход с подтяжкой вверх
	GPIO_Init (GPIOA, &GPIO_Init_UART2);

	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource2), GPIO_AF_USART2); //установим альт. функцию UART2-TX
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource3), GPIO_AF_USART2); //установим альт. функцию UART2-RX

	RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2, ENABLE); //затактируем USART2

	USART_InitTypeDef ini_USART2;
	ini_USART2.USART_BaudRate = BAUDRATE; //пропускная способность канала UART
	ini_USART2.USART_WordLength = USART_WordLength_8b; //1 стартовый бит, 8 бит данных, n стоп-бит
	ini_USART2.USART_StopBits = USART_StopBits_1; //1 стоп-бит
	ini_USART2.USART_Parity = USART_Parity_No; //без проверки бита чётности
	ini_USART2.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //включим приём/передачу по USART2
	ini_USART2.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //аппаратное управление потоком отключено
	USART_Init(USART2, &ini_USART2); //запишем настройки USART2

/*	USART_ITConfig(USART2, USART_IT_TXE, ENABLE); //включим прерывание  на передачу при пустом регистре данных
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //включим прерывание на приём при полном регистре данных
	USART_ClearITPendingBit(USART2, USART_IT_RXNE); //очистим флаг

	NVIC_EnableIRQ(USART2_IRQn); //разрешим прерывание от USART2
	__enable_irq ();  //разрешим прерывания глобально*/
	USART_Cmd(USART2, ENABLE);  //включим USART2
	}

//-------------------------функция инициализации UART3-------------------------------//
void UART3_ini ()

	{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB, ENABLE); //затактируем порт B

	GPIO_InitTypeDef 	GPIO_Init_UART3;
	GPIO_Init_UART3.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //PB10 - Tx, PB11 - Rx
	GPIO_Init_UART3.GPIO_Mode = GPIO_Mode_AF; //порт B пин 10, 11 на альт. выход
	GPIO_Init_UART3.GPIO_Speed = GPIO_Fast_Speed; //скорость порта высокая
	GPIO_Init_UART3.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_Init_UART3.GPIO_PuPd = GPIO_PuPd_UP; //вход с подтяжкой вверх
	GPIO_Init (GPIOB, &GPIO_Init_UART3);

	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource10), GPIO_AF_USART3); //установим альт. функцию UART3-TX
	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource11), GPIO_AF_USART3); //установим альт. функцию UART3-RX

	RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART3, ENABLE); //затактируем USART3

	USART_InitTypeDef ini_USART3;
	ini_USART3.USART_BaudRate = BAUDRATE; //пропускная способность канала UART
	ini_USART3.USART_WordLength = USART_WordLength_8b; //1 стартовый бит, 8 бит данных, n стоп-бит
	ini_USART3.USART_StopBits = USART_StopBits_1; //1 стоп-бит
	ini_USART3.USART_Parity = USART_Parity_No; //без проверки бита чётности
	ini_USART3.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //включим приём/передачу по USART3
	ini_USART3.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //аппаратное управление потоком отключено
	USART_Init(USART3, &ini_USART3); //запишем настройки USART3

/*	USART_ITConfig(USART3, USART_IT_TXE, ENABLE); //включим прерывание  на передачу при пустом регистре данных
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //включим прерывание на приём при полном регистре данных
	USART_ClearITPendingBit(USART3, USART_IT_RXNE); //очистим флаг

	NVIC_EnableIRQ(USART3_IRQn); //разрешим прерывание от USART3
	__enable_irq ();  //разрешим прерывания глобально*/
	USART_Cmd(USART3, ENABLE);  //включим USART3
	}

//-----------------------------функции передачи данных по UART3----------------------------//
void UART3_PutString(char *str)
{
	char c;
	while((c = *str++)) // отправл§ем символы строки последовательно, пока не достигнут символ конца строки "\0"
		{
		while (!USART_GetFlagStatus(USART3, USART_FLAG_TC)) {}; //ждЄм выставлени§ флага "transmission complete"
		USART_SendData(USART3, c); // передаем байт
		}
}
