/*
 * UART_f103.c
 *
 *  Created on: 19 июл. 2021 г.
 *      Author: Пользователь
 */

#include "UART_f103.h"

//-------------------------функция инициализации UART2-------------------------------//
void UART2_ini ()

	{
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);  //затактируем порт А

	GPIO_InitTypeDef 	GPIO_Init_UART2;
	GPIO_Init_UART2.GPIO_Pin = GPIO_Pin_2; //PA2 - Tx
	GPIO_Init_UART2.GPIO_Mode = GPIO_Mode_AF_PP; //PA2 на альт. выход с подтяжкой
	GPIO_Init_UART2.GPIO_Speed = GPIO_Speed_10MHz; //скорость порта
	GPIO_Init (GPIOA, &GPIO_Init_UART2);

	GPIO_Init_UART2.GPIO_Pin =  GPIO_Pin_3; //PA3 - Rx
	GPIO_Init_UART2.GPIO_Mode = GPIO_Mode_IN_FLOATING; //PA3 на плавающий выход
	GPIO_Init_UART2.GPIO_Speed = GPIO_Speed_10MHz; //скорость порта
	GPIO_Init (GPIOA, &GPIO_Init_UART2);

	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);

	RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2, ENABLE); //затактируем USART2

	USART_InitTypeDef ini_USART2;
	ini_USART2.USART_BaudRate = BAUDRATE; //пропускная способность UART2
	ini_USART2.USART_WordLength = USART_WordLength_8b; //1 стартовый бит, 8 бит данных, n стоп-бит
	ini_USART2.USART_StopBits = USART_StopBits_1; //1 стоп-бит
	ini_USART2.USART_Parity = USART_Parity_No; //без проверки бита чётности
	ini_USART2.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //включим приём/передачу по USART2
	ini_USART2.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //аппаратное управление потоком отключено
	USART_Init(USART2, &ini_USART2); //запишем настройки USART2

/*	USART_ITConfig(USART2, USART_IT_TXE, ENABLE); //включим прерывание  на передачу при пустом регистре данных
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //включим прерывание на приём при полном регистре данных
	NVIC_EnableIRQ(USART2_IRQn); //разрешим прерывание от USART2*/

	USART_Cmd(USART2, ENABLE);  //включим USART2
	}

//-------------------------функция инициализации UART1-------------------------------//
void UART1_ini ()

	{
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE); //затактируем порт А

	GPIO_InitTypeDef 	GPIO_Init_UART1;
	GPIO_Init_UART1.GPIO_Pin = GPIO_Pin_9; //PA9 - Tx
	GPIO_Init_UART1.GPIO_Mode = GPIO_Mode_AF_PP; //PA9 на альт. выход с подтяжкой
	GPIO_Init_UART1.GPIO_Speed = GPIO_Speed_10MHz; //скорость порта
	GPIO_Init (GPIOA, &GPIO_Init_UART1);

	GPIO_Init_UART1.GPIO_Pin =  GPIO_Pin_10; //PA10 - Rx
	GPIO_Init_UART1.GPIO_Mode = GPIO_Mode_IN_FLOATING; //PA10 на плавающий вход
	GPIO_Init_UART1.GPIO_Speed = GPIO_Speed_10MHz; //скорость порта
	GPIO_Init (GPIOA, &GPIO_Init_UART1);

//	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

	RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1, ENABLE); //затактируем USART1

	USART_InitTypeDef ini_USART1;
	ini_USART1.USART_BaudRate = BAUDRATE; //пропускная способность UART2
	ini_USART1.USART_WordLength = USART_WordLength_8b; //1 стартовый бит, 8 бит данных, n стоп-бит
	ini_USART1.USART_StopBits = USART_StopBits_1; //1 стоп-бит
	ini_USART1.USART_Parity = USART_Parity_No; //без проверки бита чётности
	ini_USART1.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //включим приём/передачу по USART1
	ini_USART1.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //аппаратное управление потоком отключено
	USART_Init(USART1, &ini_USART1); //запишем настройки USART1

/*	USART_ITConfig(USART1, USART_IT_TXE, ENABLE); //включим прерывание  на передачу при пустом регистре данных
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //включим прерывание на приём при полном регистре данных
	NVIC_EnableIRQ(USART1_IRQn); //разрешим прерывание от USART1*/

	USART_Cmd(USART1, ENABLE);  //включим USART2
	}
