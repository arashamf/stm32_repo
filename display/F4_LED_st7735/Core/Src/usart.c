/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
extern char UART3_msg_TX [UART_BUFFER_SIZE]; //буффер для передачи сообщений по UART3 
extern char UART3_msg_RX [UART_BUFFER_SIZE];
/* USER CODE END 0 */

UART_HandleTypeDef huart3;

/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 57600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//----------------------------------------------------------------------------------------------------//
uint16_t USART_ReceiveData(USART_TypeDef* USARTx)
{
  /* Check the parameters */
  assert_param(IS_USART_ALL_PERIPH(USARTx));
  
  /* Receive Data */
  return (uint16_t)(USARTx->DR & (uint16_t)0x01FF);
}

//----------------------------------------------------------------------------------------------------//
void UART3_SendByte(char b)
{
	int timeout = 300000;
	while ((USART3->SR & UART_FLAG_TXE) == (uint16_t)RESET)
	{
		if(timeout--==0)
			return;
	}
	if ((USART3->SR & USART_SR_TC) == USART_SR_TC)
	{
		/* Transmit Data */
		USART3->DR = (b & (uint16_t)0x01FF);
	}
	//wait for trasmitt
	while ((USART3->SR & UART_FLAG_TC) == (uint16_t)RESET){}		
}

//----------------------------------------------------------------------------------------------------//
void UART3_SendString (const char * text)
{
	while(*text)
	{
		UART3_SendByte(*text);
		text++;
	}
}

//----------------------------------------------------------------------------------------------------//
void USART3_IRQHandler(void) {
	 
	/*	LED_GREEN(1);
		UART3_msg_RX[counter_msg_UART3] = (uint8_t)USART_ReceiveData(USART3); 
		USART3->RQR &= ~USART_RQR_RXFRQ; //сброс флаг прерывания
		if ((counter_msg_UART3++) >= 15) //если произошло переполнение приёмного буффера
		{
			counter_msg_UART3 = 0; //обнуление указателя на приёмный буффер
			memset(UART3_msg_RX, '\0', sizeof(UART3_msg_RX)); //очистим приёмный буффер
		}
		if((UART3_msg_RX [counter_msg_UART3-2] == 0x0D) && (UART3_msg_RX [counter_msg_UART3-1] == 0x0A)) //признаки конца сообщения
		{
			counter_msg_UART3 = 0; //обнуление указателя на приёмный буффер
			recieve_flag = 1; //установка флага принятия данных
		}
		LED_GREEN(0);*/
}
//----------------------------------------------------------------------------------------------------//

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
