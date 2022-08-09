/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

extern uint8_t I2C_RTC_buffer [8];

extern unsigned int counter_msg_UART3; //счётчик элемента массива
extern unsigned int counter_msg_UART1;
extern uint8_t recieve_flag;  //флаг принятия сообщения по УАРТ

extern FIL logfile;     //файловые объекты 
extern char wtext[TXT_BUFFER_SIZE];  //буффер записи на SD карту
/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115741;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}
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
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = TXD_Pin|RXD_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, TXD_Pin|RXD_Pin);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8|GPIO_PIN_9);

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
  return (uint16_t)(USARTx->RDR & (uint16_t)0x01FF);
}

//----------------------------------------------------------------------------------------------------//
void UART3_SendByte(char b)
{
	int timeout = 300000;
	while ((USART3->ISR & UART_FLAG_TXE) == (uint16_t)RESET)
	{
		if(timeout--==0)
			return;
	}
	if ((USART3->ISR & USART_ISR_TC) == USART_ISR_TC)
	{
		/* Transmit Data */
		USART3->TDR = (b & (uint16_t)0x01FF);
	}
	//wait for trasmitt
	while ((USART3->ISR & UART_FLAG_TC) == (uint16_t)RESET){}		
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
	 
		LED_GREEN(1);
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
		LED_GREEN(0);
}

//----------------------------------------------------------------------------------------------------//
void UART1_SendByte(char b)
{
	int timeout = 300000;
	while ((USART1->ISR & UART_FLAG_TXE) == (uint16_t)RESET)
	{
		if(timeout--==0)
			return;
	}
	if ((USART1->ISR & USART_ISR_TC) == USART_ISR_TC)
	{
		/* Transmit Data */
		USART1->TDR = (b & (uint16_t)0x01FF);
	}
	//wait for trasmitt
	while ((USART1->ISR & UART_FLAG_TC) == (uint16_t)RESET){}		
}

//----------------------------------------------------------------------------------------------------//
void UART1_SendString (const char * text)
{
	while(*text)
	{
		UART1_SendByte(*text);
		text++;
	}
}

//----------------------------------------------------------------------------------------------------//
void USART1_IRQHandler(void) {
	
		LED_GREEN(1);
		unsigned char data=0;
		data = (unsigned char)USART_ReceiveData(USART1);
//		USART1->RQR &= ~USART_RQR_RXFRQ; //сброс флаг прерывания
		UART1_msg_RX [counter_msg_UART1++] = data;
		if(counter_msg_UART1 > 5)
		{
			counter_msg_UART1 = 0;
			sprintf (UART3_msg_TX, "%x %c%c%c%c%c - answer_cell\r\n", UART1_msg_RX [0], UART1_msg_RX [1], UART1_msg_RX [2], UART1_msg_RX [3], UART1_msg_RX [4], UART1_msg_RX [5]);
			UART3_SendString (UART3_msg_TX);
			sprintf (wtext, "%u:%u:%u - %x%c%c%c%c%c\r\n", I2C_RTC_buffer[2], I2C_RTC_buffer[1], I2C_RTC_buffer[0],  UART1_msg_RX [0], UART1_msg_RX [1], UART1_msg_RX [2], UART1_msg_RX [3], UART1_msg_RX [4], UART1_msg_RX [5]);
			write_reg (&logfile, wtext); //запись полученного ответа на SD-карту
			
			USART1->CR1 &= ~USART_CR1_UE; //выключение UART1
		}
		LED_GREEN(0);
}
//----------------------------------------------------------------------------------------------------//
void parse_command ()
{
	if (!(strncmp (UART3_msg_RX, "time:", 5))) //если первые 5 символов принятого сообщения равно "time:"
	{
		edit_RTC_data (hi2c1, DEV_ADDR);
	}	
		else
	{
		if (!(strncmp (UART3_msg_RX, "cell:", 5))) 
		{
			command_to_cell ();
		}	
		else
		{
			sprintf (UART3_msg_TX , "%s-command_error\r\n", UART3_msg_RX);	
			UART3_SendString (UART3_msg_TX);
		}
	}
	memset(UART3_msg_RX, '\0', sizeof(UART3_msg_RX)); //очистка приёмного буффера
}

//----------------------------------------------------------------------------------------------------//
void command_to_cell ()
{	
	char *ptr;	char buffer [3];
	uint8_t errflag = 1; //флаг ошибки данных
	ptr = strtok(UART3_msg_RX , ": "); //Ф-ия возвращает указатель на первую найденную лексему в строке. Если не найдено, то возвращается пустой указатель
	ptr = strtok(NULL, "tc \r\n"); // для последующего вызова можно передать NULL, тогда функция продолжит поиск в оригинальной строке
	strncpy (buffer, ptr, 3);  //копирование 3 символов цифр в массив для отправки	
	if (strlen (buffer) < 3) //проверка наличия в буффере символов не меньше 3
		{
		sprintf (UART3_msg_TX , "error! need_more_numbers\r\n");	
		UART3_SendString (UART3_msg_TX);
		errflag = 0;
		}
	for (uint8_t count = 0; count < 3; count++) //проверка чисел на корректность значений
	{
		if (count == 0)
		{
			if (!((buffer [count] > 47) && (buffer [count] < 50))) //если символ меньше 0 и больше 1
			{
				errflag = 0; //неверный формат данных команды
				sprintf (UART3_msg_TX , "%c-invalid_command_format\r\n", buffer [count]);	
				UART3_SendString (UART3_msg_TX);
				break;
			}
		}
		else
		{
			if (count == 1)
			{
				if (!((buffer [count] > 47) && (buffer [count] < 58))) //если символ меньше 0 и больше 9
				{
					errflag = 0; //неверный формат данных команды
					sprintf (UART3_msg_TX , "%c-invalid_command_format\r\n", buffer [count]);	
					UART3_SendString (UART3_msg_TX);
					break;
				}
			}
			else
			{
				if (!((buffer [count] > 47) && (buffer [count] < 51))) //если символ меньше 0 и больше 2
				{
					errflag = 0; //неверный формат данных команды
					sprintf (UART3_msg_TX , "%c-invalid_command_format\r\n", buffer [count]);	
					UART3_SendString (UART3_msg_TX);
					break;
				}
			}
		}
	}
	if (errflag)
	{
		UART1_msg_TX [0] = 0x2; UART1_msg_TX [1] = buffer [0]; UART1_msg_TX [2] = buffer [1]; UART1_msg_TX [3] = buffer [2]; //формирование сообщения
		sprintf (wtext, "%u:%u:%u - %x%c%c%c\r\n", I2C_RTC_buffer[2], I2C_RTC_buffer[1], I2C_RTC_buffer[0], UART1_msg_TX[0], UART1_msg_TX [1], UART1_msg_TX [2], UART1_msg_TX [3]);
		write_reg (&logfile, wtext); //запись отправленной команды на SD-карту
		USART1->CR1 |= USART_CR1_UE; //включение UART1
		RS485_TX; //режим на передачу
		UART1_SendString (UART1_msg_TX); //передача сообщения
		RS485_RX; //режим на приём
	}
	memset(UART3_msg_RX, '\0', sizeof(UART3_msg_RX)); //очистка приёмного буффера
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
