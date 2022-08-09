/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "gpio.h"
#include "st7735.h"
#include "rtc.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */
extern char UART3_msg_TX [UART_BUFFER_SIZE]; //буффер для передачи сообщений по UART3 
extern char	UART3_msg_RX [30]; //буффер для приёма сообщений по UART3
extern char UART1_msg_RX [6]; //буффер для приёма сообщений по UART1
extern char UART1_msg_TX [4]; //буффер для передачи сообщений по UART1
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void UART3_SendByte(char );
void UART3_SendString (const char *);
void UART1_SendByte(char );
void UART1_SendString (const char *);
uint16_t USART_ReceiveData(USART_TypeDef* USARTx);
void parse_command (void);
void command_to_cell (void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
