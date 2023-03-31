/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include "usart.h"
#include "stdio.h"
#include "gpio.h"

CAN_TxHeaderTypeDef TxHeader; //структура TxHeader отвечает за отправку кадров
CAN_RxHeaderTypeDef CAN1_Rx_buf; //структура для приёма сообщения CAN1
CAN_RxHeaderTypeDef CAN2_Rx_buf; //структура для приёма сообщения CAN2

uint32_t TxMailbox = 0;//номер почтового ящика для отправки
uint8_t CAN_RxData [10]; //буффер принятого сообщения по CAN
/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 21;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = ENABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = ENABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */
	CAN_FilterTypeDef can1_filter;
	
	can1_filter.FilterBank = 0; //номер фильтра
	can1_filter.FilterIdHigh = 0x0000; // старшая часть первого регистра фильтра
	can1_filter.FilterIdLow = 0x0000; // младшая часть первого регистра фильтра
	can1_filter.FilterMaskIdHigh = 0x0000; // старшая часть маски фильтра
	can1_filter.FilterMaskIdLow = 0x0000; // младшая часть маски фильтра
	can1_filter.FilterFIFOAssignment = CAN_RX_FIFO0; //настройка фильтра для приёмного буфера CAN_RX_FIFO0
	can1_filter.FilterMode = CAN_FILTERMODE_IDMASK; //режим работы фильтра
	can1_filter.FilterScale =  CAN_FILTERSCALE_32BIT; //размерность фильтра, 32 бита - фильтроваться могут либо стандартные (11 бит) идентификаторы, либо расширенные (29 бит)
	can1_filter.FilterActivation = ENABLE;
	
	if(HAL_CAN_ConfigFilter(&hcan1, &can1_filter) != HAL_OK)
  {
    sprintf (buffer_TX_UART3, (char *)"init_CAN1_filter_error\r\n");
		UART3_PutString (buffer_TX_UART3);
		Error_Handler();
  }
	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_ERROR | CAN_IT_BUSOFF | CAN_IT_LAST_ERROR_CODE);
  /* USER CODE END CAN1_Init 2 */

}
/* CAN2 init function */
void MX_CAN2_Init(void)
{

  /* USER CODE BEGIN CAN2_Init 0 */

  /* USER CODE END CAN2_Init 0 */

  /* USER CODE BEGIN CAN2_Init 1 */

  /* USER CODE END CAN2_Init 1 */
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 12;
  hcan2.Init.Mode = CAN_MODE_SILENT_LOOPBACK;
  hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = ENABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = DISABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = ENABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN2_Init 2 */
	CAN_FilterTypeDef can2_filter;
	
	can2_filter.FilterBank = 14; //номер фильтра
	can2_filter.FilterIdHigh = 0x0000; // старшая часть первого регистра фильтра
	can2_filter.FilterIdLow = 0x0000; // младшая часть первого регистра фильтра
	can2_filter.FilterMaskIdHigh = 0x0000; // старшая часть маски фильтра
	can2_filter.FilterMaskIdLow = 0x0000; // младшая часть маски фильтра
	can2_filter.FilterFIFOAssignment = CAN_RX_FIFO0; //настройка фильтра для приёмного буфера CAN_RX_FIFO0
	can2_filter.FilterMode = CAN_FILTERMODE_IDMASK; //режим работы фильтра
	can2_filter.FilterScale =  CAN_FILTERSCALE_32BIT; //размерность фильтра, 32 бита - фильтроваться могут либо стандартные (11 бит) идентификаторы, либо расширенные (29 бит)
	can2_filter.FilterActivation = ENABLE;
	can2_filter.SlaveStartFilterBank = 14; //этот параметр сообщает системе с какого номера начинаются фильтры CAN2 (обязателен при использовании двух CAN)
	
	if(HAL_CAN_ConfigFilter(&hcan2, &can2_filter) != HAL_OK)
  {
    sprintf (buffer_TX_UART3, (char *)"init_CAN2_filter_error\r\n");
		UART3_PutString (buffer_TX_UART3);
		Error_Handler();
  }
	HAL_CAN_Start(&hcan2);
	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_ERROR | CAN_IT_BUSOFF | CAN_IT_LAST_ERROR_CODE);
  /* USER CODE END CAN2_Init 2 */

}

static uint32_t HAL_RCC_CAN1_CLK_ENABLED=0;

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    HAL_RCC_CAN1_CLK_ENABLED++;
    if(HAL_RCC_CAN1_CLK_ENABLED==1){
      __HAL_RCC_CAN1_CLK_ENABLE();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
  else if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspInit 0 */

  /* USER CODE END CAN2_MspInit 0 */
    /* CAN2 clock enable */
    __HAL_RCC_CAN2_CLK_ENABLE();
    HAL_RCC_CAN1_CLK_ENABLED++;
    if(HAL_RCC_CAN1_CLK_ENABLED==1){
      __HAL_RCC_CAN1_CLK_ENABLE();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* CAN2 interrupt Init */
    HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN2_SCE_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN2_SCE_IRQn);
  /* USER CODE BEGIN CAN2_MspInit 1 */

  /* USER CODE END CAN2_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    HAL_RCC_CAN1_CLK_ENABLED--;
    if(HAL_RCC_CAN1_CLK_ENABLED==0){
      __HAL_RCC_CAN1_CLK_DISABLE();
    }

    /**CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
  else if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspDeInit 0 */

  /* USER CODE END CAN2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN2_CLK_DISABLE();
    HAL_RCC_CAN1_CLK_ENABLED--;
    if(HAL_RCC_CAN1_CLK_ENABLED==0){
      __HAL_RCC_CAN1_CLK_DISABLE();
    }

    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13);

    /* CAN2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN2_SCE_IRQn);
  /* USER CODE BEGIN CAN2_MspDeInit 1 */

  /* USER CODE END CAN2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//----------------------------------коллбэк для буфера приёма FIFO №0----------------------------------//
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) 
{
	if(HAL_CAN_GetRxMessage (&hcan1, CAN_RX_FIFO0, &CAN1_Rx_buf, CAN_RxData) == HAL_OK) //если пришло прерывание получения пакета в буфер FIFO0 CAN1
  {
		
		sprintf (buffer_TX_UART3, "id:%x, msg:%c%c%c%c%c%c%c%c\r\n", CAN1_Rx_buf.StdId, CAN_RxData[0], CAN_RxData[1], 
		CAN_RxData[2], CAN_RxData[3], CAN_RxData[4], CAN_RxData[5], CAN_RxData[6], CAN_RxData[7]);	
		UART3_PutString (buffer_TX_UART3);
		LED_RED (0);
  }	
	if(HAL_CAN_GetRxMessage (&hcan2, CAN_RX_FIFO0, &CAN2_Rx_buf, CAN_RxData) == HAL_OK) //если пришло прерывание получения пакета в буфер FIFO0 CAN1
  {
		
		sprintf (buffer_TX_UART3, "id:%x, msg:%c%c%c%c%c%c%c%c\r\n", CAN1_Rx_buf.StdId, CAN_RxData[0], CAN_RxData[1], 
		CAN_RxData[2], CAN_RxData[3], CAN_RxData[4], CAN_RxData[5], CAN_RxData[6], CAN_RxData[7]);	
		UART3_PutString (buffer_TX_UART3);
		LED_GREEN (0);
  }	
}

//----------------------------------коллбэк для буфера приёма FIFO №1----------------------------------//
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) 
{
	if(HAL_CAN_GetRxMessage (&hcan1, CAN_RX_FIFO1, &CAN1_Rx_buf, CAN_RxData) == HAL_OK) //если пришло прерывание получения пакета в буфер FIFO0 CAN1
  {
		
		sprintf (buffer_TX_UART3, "can1:%x%x%x%x%x%x%x%x\r\n", CAN_RxData[0], CAN_RxData[1], CAN_RxData[2], CAN_RxData[3], CAN_RxData[4], CAN_RxData[5], CAN_RxData[6], CAN_RxData[7]);	
		UART3_PutString (buffer_TX_UART3);
		LED_RED (0);
  }	
	if(HAL_CAN_GetRxMessage (&hcan2, CAN_RX_FIFO1, &CAN2_Rx_buf, CAN_RxData) == HAL_OK) //если пришло прерывание получения пакета в буфер FIFO0 CAN1
  {
		
		sprintf (buffer_TX_UART3, "can2:%x%x%x%x%x%x%x%x\r\n", CAN_RxData[0], CAN_RxData[1], CAN_RxData[2], CAN_RxData[3], CAN_RxData[4], CAN_RxData[5], CAN_RxData[6], CAN_RxData[7]);	
		UART3_PutString (buffer_TX_UART3);
		LED_GREEN (0);
  }	
}


//----------------------------------------коллбек ошибки по переполнению Fifo0----------------------------------------//
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan)
{
	sprintf (buffer_TX_UART3, (char *)"CAN_Fifo0_Full");
	UART3_PutString (buffer_TX_UART3);
}

//----------------------------------------коллбек ошибок CAN----------------------------------------//
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	uint32_t errorcode = 0;
	if ((errorcode = HAL_CAN_GetError(&hcan1)) != HAL_OK)
	{
		sprintf (buffer_TX_UART3, (char *)"CAN1_ERROR=%x\r\n", errorcode);
		UART3_PutString (buffer_TX_UART3);
	}
	if ((errorcode = HAL_CAN_GetError(&hcan2)) != HAL_OK)
	{
		sprintf (buffer_TX_UART3, (char *)"CAN2_ERROR=%x\r\n", errorcode);
		UART3_PutString (buffer_TX_UART3);
	}
}	

//--------------------------------------------------------------------------------//
void CAN1_Send_Message (uint8_t * CAN_TxData)
{
	uint32_t errorcode;
	uint8_t CAN_Tx_buffer[8]; 
	uint32_t uwCounter = 0;
	
	LED_RED (1);
	
	TxHeader.StdId = 0x10F; //ID заголовка
	TxHeader.ExtId = 8;
	TxHeader.RTR = CAN_RTR_DATA; //тип сообщения (CAN_RTR_Data - передача данных)
	TxHeader.IDE = CAN_ID_STD;   //формат кадра Standard
	TxHeader.DLC = 8; //количество байт в сообщении
	TxHeader.TransmitGlobalTime = 0;
	
	for (uint8_t count = 0; count < 8; count++)
	{
		CAN_Tx_buffer[count] = *(CAN_TxData + count);}
	
	while ((HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) && (uwCounter != 0xFFFF)) //ожидание освобождения TxMailbox
	{
		uwCounter++;
	} 
	if (uwCounter == 0xFFFF)	//выход по тайм-ауту
	{
		sprintf (buffer_TX_UART3, "CAN1_TX_time_out\r\n");
		UART3_PutString (buffer_TX_UART3);
		return;
	}
	
	if (READ_BIT (CAN1->TSR, CAN_TSR_TME0)) {
		TxMailbox = 0;}
	else
	{
		if (READ_BIT (CAN1->TSR, CAN_TSR_TME1)) {
		TxMailbox = 1;}
		else
		{
			if (READ_BIT (CAN1->TSR, CAN_TSR_TME2)) {
			TxMailbox = 2;}
		}
	}
	
	if ((errorcode = HAL_CAN_AddTxMessage(&hcan1, &TxHeader, CAN_Tx_buffer, &TxMailbox)) != HAL_OK) //Добавление сообщений в первый свободный Mailboxe Tx и активация запроса на передачу
	{	 
		sprintf (buffer_TX_UART3, "CAN1_TX_ERROR=%u", errorcode);
		UART3_PutString (buffer_TX_UART3);
	}
}

//--------------------------------------------------------------------------------//
void CAN1_Send_C1 ()
{
	uint32_t errorcode;
	uint8_t CAN_Tx_buffer[8]; 
	uint32_t uwCounter = 0;
	
	LED_RED (1);
	
	TxHeader.StdId = (ID_C1<<5) | ID_adress; //ID заголовка
	TxHeader.ExtId = 0;
	TxHeader.RTR = CAN_RTR_REMOTE; //тип сообщения (CAN_RTR_Data - передача данных)
	TxHeader.IDE = CAN_ID_STD;   //формат кадра Standard
	TxHeader.DLC = 0; //количество байт в сообщении
	TxHeader.TransmitGlobalTime = 0;
	
	while ((HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) && (uwCounter != 0xFFFF)) //ожидание освобождения TxMailbox
	{
		uwCounter++;
	} 
	if (uwCounter == 0xFFFF)	//выход по тайм-ауту
	{
		sprintf (buffer_TX_UART3, "CAN1_TX_time_out\r\n");
		UART3_PutString (buffer_TX_UART3);
		return;
	}
	
	if (READ_BIT (CAN1->TSR, CAN_TSR_TME0)) {
		TxMailbox = 0;}
	else
	{
		if (READ_BIT (CAN1->TSR, CAN_TSR_TME1)) {
		TxMailbox = 1;}
		else
		{
			if (READ_BIT (CAN1->TSR, CAN_TSR_TME2)) {
			TxMailbox = 2;}
		}
	}
	
	if ((errorcode = HAL_CAN_AddTxMessage(&hcan1, &TxHeader, CAN_Tx_buffer, &TxMailbox)) != HAL_OK) //Добавление сообщений в первый свободный Mailboxe Tx и активация запроса на передачу
	{	 
		sprintf (buffer_TX_UART3, "CAN1_TX_ERROR=%u", errorcode);
		UART3_PutString (buffer_TX_UART3);
	}
}

//--------------------------------------------------------------------------------//
void CAN2_Send_Message (uint8_t * CAN_TxData)
{
	uint32_t errorcode;
	uint8_t CAN_Tx_buffer[8];
	uint32_t uwCounter = 0;
	
	LED_GREEN (1);
	
	TxHeader.StdId = 0x10F; //ID заголовка
	TxHeader.ExtId = 0;
	TxHeader.RTR = CAN_RTR_DATA; //тип сообщения (CAN_RTR_Data - передача данных)
	TxHeader.IDE = CAN_ID_STD;   //формат кадра Standard
	TxHeader.DLC = 8; //количество байт в сообщении
	TxHeader.TransmitGlobalTime = 0;
	
	for (uint8_t count = 0; count < 8; count++)
	{
		CAN_Tx_buffer[count] = *(CAN_TxData + count);}
	
	while ((HAL_CAN_GetTxMailboxesFreeLevel(&hcan2) == 0) && (uwCounter != 0xFFFF)) //ожидание освобождения TxMailbox
	{
		uwCounter++;
	} 
	if (uwCounter == 0xFFFF)	//выход по тайм-ауту
	{
		sprintf (buffer_TX_UART3, "CAN2_TX_time_out");
		UART3_PutString (buffer_TX_UART3);
		return;
	}
		
	if (READ_BIT (CAN2->TSR, CAN_TSR_TME0)) {
		TxMailbox = 0;}
	else
	{
		if (READ_BIT (CAN2->TSR, CAN_TSR_TME1)) {
		TxMailbox = 1;}
		else
		{
			if (READ_BIT (CAN2->TSR, CAN_TSR_TME2)) {
			TxMailbox = 2;}
		}
	}
	
	if ((errorcode = HAL_CAN_AddTxMessage(&hcan2, &TxHeader, CAN_Tx_buffer, &TxMailbox)) != HAL_OK) //Добавление сообщений в первый свободный Mailboxe Tx и активация запроса на передачу
	{	 
		sprintf (buffer_TX_UART3, "CAN2_TX_ERROR=%u", errorcode);
		UART3_PutString (buffer_TX_UART3);
	}
}
/* USER CODE END 1 */
