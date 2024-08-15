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
#include <stdio.h>
#include <stdbool.h>
#include <string.h> 
#include "usart.h"
#include "pins.h"
#include "protocol.h"

typedef struct
{
	uint8_t flag_RX;
	uint8_t RxData[8];
}
CAN_RX_msg;

extern MKS2_t MKS2;

//Macro -----------------------------------------------------------------------------------------------//
#define MAKE_FRAME_ID( msg_type_id, board_addr) ((((uint32_t)msg_type_id) << 5) | (board_addr)) //��������� ID ���������
#define MAKE_MSG_DATA0(__module_id, __data_type) ( ( __module_id << 3 ) | __data_type ) //������� 5 ��� 1 ����� ��������� ��� ������-����������� ��� ��� ������ ����������, ������� 3 ���� - ��� ���� ������
#define GET_MODULE_ADDR( frame_id) ((frame_id) & 0x1F) //���������� ����� ������ (5 ���) �� ��������� CAN ���������
#define GET_MSG_TYPE( frame_id) (((frame_id) >> 5) & 0x3F) //��� ��������� (6 ���) �� ��������� CAN ���������

//Private variables -----------------------------------------------------------------------------------//
static CAN_TxHeaderTypeDef CAN_TxHeader; //��������� TxHeader �������� �� �������� ������
static CAN_RxHeaderTypeDef CAN_RxHeader; //��������� ��� ����� ��������� CAN1

static CAN_RX_msg CAN1_RX;
static uint32_t TxMailbox = 0;//����� ��������� ����� ��� �������� 

static MESSAGE_C2_t MESSAGE_C2; //��������� ��� ��������� ���� �2
static MESSAGE_A1_t MESSAGE_A1; //��������� ��� ��������� �1
//static MESSAGE_B_CONFIG_t MESSAGE_B_CONFIG; //��������� ��� ����������� ����������������� ��������� ���� �
//static MESSAGE_B_SERVICE_t MESSAGE_B_SERVICE; //��������� ��� ����������� ������������ ��������� ���� �

/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 18;
  hcan.Init.Mode = CAN_MODE_SILENT_LOOPBACK;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = ENABLE;
  hcan.Init.AutoWakeUp = ENABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = ENABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */
	//---------------------------------------��������� ������� ��� FIFO0--------------------------------------//
	CAN_FilterTypeDef can_FIFO_filter;
	
	can_FIFO_filter.FilterBank = 0; //����� �������
	can_FIFO_filter.FilterIdHigh =((MAKE_FRAME_ID(MSG_TYPE_C, (uint8_t)MKS2.canContext.Addr))<<5); // ������� ����� ������� �������� �������
	can_FIFO_filter.FilterIdLow = ((MAKE_FRAME_ID(MSG_TYPE_C, (uint8_t)MKS2.canContext.Addr))<<5); // ������� ����� ������� �������� �������
//	can_FIFO_filter.FilterMaskIdHigh = 0x7E0<<5; // ������� ����� ����� �������
	can_FIFO_filter.FilterMaskIdHigh = ((MAKE_FRAME_ID(MSG_TYPE_C, (uint8_t)MKS2.canContext.Addr))<<5);
	can_FIFO_filter.FilterMaskIdLow = ((MAKE_FRAME_ID(MSG_TYPE_C, (uint8_t)MKS2.canContext.Addr))<<5); // ������� ����� ����� �������
	can_FIFO_filter.FilterFIFOAssignment = CAN_RX_FIFO0; //��������� ������� ��� �������� ������ CAN_RX_FIFO0
	can_FIFO_filter.FilterMode = CAN_FILTERMODE_IDLIST; //����� ������ �������
	can_FIFO_filter.FilterScale =  CAN_FILTERSCALE_16BIT; //����������� �������, 32 ���� - ������������� ����� ���� ����������� (11 ���) ��������������, ���� ����������� (29 ���)
	can_FIFO_filter.FilterActivation = ENABLE;
	
	if(HAL_CAN_ConfigFilter(&hcan, &can_FIFO_filter) != HAL_OK)
		{Error_Handler();}
	
	//---------------------------------------��������� ������� ��� FIFO1--------------------------------------//
	can_FIFO_filter.FilterBank = 1; //����� �������
	can_FIFO_filter.FilterIdHigh = ((MAKE_FRAME_ID(MSG_TYPE_A1, (uint8_t)MKS2.canContext.Addr))<<5); // ������� ����� ������� �������� �������
	can_FIFO_filter.FilterIdLow = ((MAKE_FRAME_ID(MSG_TYPE_A1, (uint8_t)MKS2.canContext.Addr))<<5); // ������� ����� ������� �������� �������

	can_FIFO_filter.FilterMaskIdHigh = ((MAKE_FRAME_ID(MSG_TYPE_A1, (uint8_t)MKS2.canContext.Addr))<<5); // ������� ����� ����� �������
	can_FIFO_filter.FilterMaskIdLow = ((MAKE_FRAME_ID(MSG_TYPE_A1, (uint8_t)MKS2.canContext.Addr))<<5); // ������� ����� ����� �������
	can_FIFO_filter.FilterFIFOAssignment = CAN_RX_FIFO1; //��������� ������� ��� �������� ������ CAN_RX_FIFO1
	can_FIFO_filter.FilterActivation = ENABLE;
	
	if(HAL_CAN_ConfigFilter(&hcan, &can_FIFO_filter) != HAL_OK)
		{Error_Handler();}
		
	HAL_CAN_Start(&hcan); 
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING 
	| CAN_IT_ERROR | CAN_IT_BUSOFF | CAN_IT_LAST_ERROR_CODE); //��������� ���������� CAN
	
	CAN1_RX.flag_RX = RX_NONE; //��������� ������� ����� CAN: ��������� �� �������

  /* USER CODE END CAN_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//--------------------------------------------------------------------------------------------------------------//
void CAN1_RX_Process(void)
{	
	/*if ( CAN_GetTEC(MDR_CAN1) > 127 )  //���� ���������� ������ ������ 127
		{CAN_Cmd(MDR_CAN1, DISABLE);} //���������� CAN1
	//	����� ����������� ��������� ��� ������� ���������
	if (CAN_GetBufferStatus(MDR_CAN1, 0) & CAN_STATUS_RX_FULL) //��������� ������ ������ CAN_BUFFER_0 �� ������� �������������� ����� "����� �����"
	{
		CAN_MyGetRawReceivedData(MDR_CAN1, 0, &RxMsg); //�������� ������ �� ������
		//DBG("RxMsg.Data[0]:0x%08X;RxMsg.Data[1]:0x%08X\n\n",RxMsg.Data[0],RxMsg.Data[1]);		
		if (RxMsg.Rx_Header.DLC == 0x08 && RxMsg.Rx_Header.RTR == 0 && 
					(uint8_t)RxMsg.Data[0] == MAKE_MSG_DATA0(canContext->ID, 0)) //���� �������� ����������� ��������� C2
		{
			xTimer_Delete(xTimerRCVSelfC2RqstTimeout);
			fContext->CAN = 0;
		}	
		// ������ ��������� ������ - ��������� C1
		if (RxMsg.Rx_Header.DLC == 0 && RxMsg.Rx_Header.RTR == 1) //���� �������� ����������� ��������� C1
		{
			CAN1_C2_Send(); //�������� ��������� �2
			fContext->CAN = 0;
			xTimer_Reload(xTimerSelfC2Rqst);
		}		
		CAN_ClearFlag(MDR_CAN1, 0, CAN_STATUS_RX_FULL); //������� ���� "����� �����"
	}

	// ��������� ������ ������ CAN_BUFFER_1 �� ������� �������������� ����� "����� �����"
	if ( CAN_GetBufferStatus(MDR_CAN1, 1) & CAN_STATUS_RX_FULL ) 
	{
		CAN_MyGetRawReceivedData(MDR_CAN1, 1, &RxMsg); //�������� ������ �� ������		
		//DBG("RxMsg.Data[0]:0x%08X;RxMsg.Data[1]:0x%08X\n\n",RxMsg.Data[0],RxMsg.Data[1]);		
		// ����������� ��������� C2
		if (RxMsg.Rx_Header.DLC == 0x08 && RxMsg.Rx_Header.RTR == 0 && 
				(uint8_t)RxMsg.Data[0] == MAKE_MSG_DATA0(canContext->ID, 0)) 
		{
			xTimer_Delete(xTimerRCVSelfC2RqstTimeout);
			fContext->CAN = 0;
		}
		
		// ������ ��������� ������ - ��������� C1
		if ( RxMsg.Rx_Header.DLC == 0 && RxMsg.Rx_Header.RTR == 1 ) 
		{
			CAN1_C2_Send();
			fContext->CAN = 0;
			xTimer_Reload(xTimerSelfC2Rqst);
		}		
		CAN_ClearFlag(MDR_CAN1, 1, CAN_STATUS_RX_FULL); //������� ���� "����� �����"
	}
	//����� ��������� ���������
	// ��������� ������ ������ CAN_BUFFER_4 �� ������� �������������� ����� "����� �����"
	if ( CAN_GetBufferStatus(MDR_CAN1, 4) & CAN_STATUS_RX_FULL ) 
	{
		CAN_MyGetRawReceivedData(MDR_CAN1, 4, &RxMsg); //�������� ������ �� ������
		// ������������
		if ( (uint8_t)RxMsg.Data[0] == MAKE_MSG_DATA0(canContext->ID, DATA_TYPE_CONFIG) ) //�������� 1 ����� ���������
		{
			memcpy(&MESSAGE_B_CONFIG.RAW[0], &RxMsg.Data[0], 8);		
			cfgContext->Max_gDOP = ((float)MESSAGE_B_CONFIG.gDOP) / 100;
		}
		//��������� (������������)
		if ( (uint8_t)RxMsg.Data[0] == MAKE_MSG_DATA0(canContext->ID, DATA_TYPE_SERVICE) ) 
		{
			memcpy(&MESSAGE_B_SERVICE.RAW[0], &RxMsg.Data[0], 8);
			if ( MESSAGE_B_SERVICE.reset ) 
				{NVIC_SystemReset();}		
		}	
		CAN_ClearFlag(MDR_CAN1, 4, CAN_STATUS_RX_FULL); //������� ���� "����� �����"
	}	*/	
}

//--------------------------------------------------------------------------------------------------------------//
void MKS2_CAN_Init(void)
{
	MKS2.canContext.MsgA1Send = &Send_Message_A1;
}

//--------------------------------------������� ��� ������ ����� FIFO �0--------------------------------------//
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) 
{
	if(HAL_CAN_GetRxMessage (hcan, CAN_RX_FIFO0, &CAN_RxHeader, CAN1_RX.RxData) == HAL_OK) //���� ������ ���������� ��������� ������ � ����� FIFO0 CAN1
	{
		CAN1_RX.flag_RX = RX0; //��������� ������� ����� CAN: ������� ��������������������� ��������� 
		sprintf (buffer_TX_UART2, (char *)"FIFO0_id=%x,msg=%x_%x_%x_%x_%x_%x_%x_%x\r\n", CAN_RxHeader.StdId, CAN1_RX.RxData[0], CAN1_RX.RxData[1], 
		CAN1_RX.RxData[2], CAN1_RX.RxData[3], CAN1_RX.RxData[4], CAN1_RX.RxData[5], CAN1_RX.RxData[6], CAN1_RX.RxData[7]);
		UART2_PutString (buffer_TX_UART2);
	}	
}

//---------------------------------������� ������ �� ������������ Fifo0---------------------------------//
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan)
{
	//g_MyFlags.CAN_Fail = 1;
	sprintf (buffer_TX_UART2, (char *)"CAN_FIFO0_Full");
	UART2_PutString (buffer_TX_UART2);
}

//----------------------------------������� ��� ������ ����� FIFO �1----------------------------------//
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) 
{
	if(HAL_CAN_GetRxMessage (hcan, CAN_RX_FIFO1, &CAN_RxHeader, CAN1_RX.RxData) == HAL_OK) //���� ������ ���������� ��������� ������ � ����� FIFO0 CAN1
	{
		CAN1_RX.flag_RX = RX1; //��������� ������� ����� CAN: ������� ��������������������� ��������� 
		sprintf (buffer_TX_UART2, (char *)"FIFO1_id=%x,msg=%x_%x_%x_%x_%x_%x_%x_%x\r\n", CAN_RxHeader.StdId, CAN1_RX.RxData[0], CAN1_RX.RxData[1], 
		CAN1_RX.RxData[2], CAN1_RX.RxData[3], CAN1_RX.RxData[4], CAN1_RX.RxData[5], CAN1_RX.RxData[6], CAN1_RX.RxData[7]);
		UART2_PutString (buffer_TX_UART2);
	}	
}

//---------------------------------������� ������ �� ������������ Fifo1---------------------------------//
void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan)
{
	//g_MyFlags.CAN_Fail = 1;
	sprintf (buffer_TX_UART2, (char *)"CAN_FIFO1_Full");
	UART2_PutString (buffer_TX_UART2);
}

//------------------------------------------������� ������ CAN------------------------------------------//
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	uint32_t errorcode = 0;
	if ((errorcode = HAL_CAN_GetError(hcan)) != HAL_OK)
	{
		//g_MyFlags.CAN_Fail = 1;
		sprintf (buffer_TX_UART2, (char *)"CAN1_ERROR=%u\r\n", errorcode);
		UART2_PutString (buffer_TX_UART2);
	}
}	

//---------------------------������ ��������� C1 (�� ���) � C2 (�����������)---------------------------//
static TRxResult ReadMsgCAN(void)
{
	if (CAN1_RX.flag_RX == RX_NONE)
		{return CAN1_RX.flag_RX;}
	
	if( CAN_RxHeader.RTR == CAN_RTR_REMOTE) //���� � ���������� ��������� ���������� ��� RTR
	{
		if ((CAN_RxHeader.StdId & MKS2.canContext.Addr) == MKS2.canContext.Addr)
			{return (CAN1_RX.flag_RX = RX_C1);} //�������� ��������� �1
	}
	else
	{
		// ���� �� ���������� RTR - ��������, ��� ��� ���� ����������� ���������
		if ( (CAN_RxHeader.DLC == 8) && ( CAN_RxHeader.RTR == CAN_RTR_DATA) && ((CAN_RxHeader.StdId & MKS2.canContext.Addr) == MKS2.canContext.Addr))
			{return (CAN1_RX.flag_RX = RX_OWN_C2);}
	}
	return CAN1_RX.flag_RX;
}

//-------------------------------------------------------------------------------------------------------//
void Task_CANRX(void)
{
	static uint32_t errorcode; //��� ������ CAN
  static bool need_init = true;
  static uint32_t last_c2_tx_ticks; //static - �������� ����������� ����� �������� 
  uint32_t current_ticks;

	current_ticks = HAL_GetTick();

	if( need_init )
	{
		last_c2_tx_ticks = current_ticks; //���������� �������� �������� �������
		need_init = false;
	}
	
	switch(CAN1_RX.flag_RX)
	{
		case RX_C1: //������� ������ C1  

			/*if ((errorcode = Send_Message_C2()) != HAL_OK ) //����������� ��������� C2
				{g_MyFlags.CAN_Fail = 1;} //���� �������� �� ������� - ��������� ����� ������ CAN
			else
				{g_MyFlags.CAN_Fail = 0;}  // ����� ����� ������ CAN*/
			CAN1_RX.flag_RX = RX_NONE; //����� ����� ����������� CAN ���������
			last_c2_tx_ticks = current_ticks; //����������� ������� �������� ��������� �2
			break;

		case RX_OWN_C2: //�������� ����������� ��������� C2 

			//g_MyFlags.CAN_Fail = 0;  // ����� ����� ������ CAN
			//last_c2_rx_ticks = current_ticks; //���������� �������� ���������� �����
			CAN1_RX.flag_RX = RX_NONE; //����� ����� ����������� CAN ���������
			break;

		case RX_NONE:
		break;
		
		case RX0:
			if( CAN_RxHeader.RTR == CAN_RTR_REMOTE) //���� � ���������� ��������� ���������� ��� RTR
			{
				if ((CAN_RxHeader.StdId & MKS2.canContext.Addr) == MKS2.canContext.Addr)
				{
					CAN1_RX.flag_RX = RX_C1;  //�������� ��������� �1
					break;
				}
			}					
			CAN1_RX.flag_RX = RX_NONE; //����� ����� ����������� CAN ���������
			break;
		
		case RX1:
			CAN1_RX.flag_RX = RX_NONE; //����� ����� ����������� CAN ���������
			break;
		
		default:
		break;
	}
	
	if( current_ticks - last_c2_tx_ticks > 4*TICKS_PER_SECOND ) //���� ����� (4�) �� ���������� �2 (� ����� �� �1)
	{		
	/*	if ((errorcode = Send_Message_C2 ()) != HAL_OK ) //�������� ��������� C2 ��� �������� ����������������� CAN, ��������� ������� ��������
		{
			g_MyFlags.CAN_Fail = 1; // ��������� ����� ������ CAN
			CAN_Reinit ();	
		} */
		last_c2_tx_ticks = current_ticks; //���������� �������� ���������� �����
	}
}


//------------------------------------------------------------------------------------------------------------//
uint32_t Send_Message_C2 (void)
{
	uint32_t errorcode; //��� ������ CAN
	uint8_t *msg_flags;
	uint8_t CAN_Tx_buffer[8];

	//������������ CAN - ���������
	CAN_TxHeader.StdId = MAKE_FRAME_ID(MSG_TYPE_C, (uint8_t)MKS2.canContext.Addr); //ID ��������� ��������� �2
	CAN_TxHeader.ExtId = 0;
	CAN_TxHeader.RTR = CAN_RTR_DATA; //��� ��������� (CAN_RTR_Data - �������� ������)
	CAN_TxHeader.IDE = CAN_ID_STD;   //������ ����� Standard
	CAN_TxHeader.DLC = 0x8; //���������� ���� � ���������
	CAN_TxHeader.TransmitGlobalTime = 0;
	
	MESSAGE_C2.module_id = MKS2.canContext.ID;
	MESSAGE_C2.data_type = 0;
	
	if ( (MKS2.fContext.Fail & FAIL_MASK) != 0 ) 
		{MESSAGE_C2.fail = 1;} 
	else 
		{MESSAGE_C2.fail = 0;}

	MESSAGE_C2.fail_gps = MKS2.fContext.GPS;
	MESSAGE_C2.fail_gps_ant = MKS2.fContext.GPSAntShortCircuit;
	MESSAGE_C2.gps_ant_disc = MKS2.fContext.GPSAntDisconnect;
		
	memcpy(CAN_Tx_buffer, MESSAGE_C2.RAW, 8);		
	CAN1_Send_Message (&CAN_TxHeader, CAN_Tx_buffer);
		
	return errorcode;
}

//------------------------------------------------------------------------------------------------------------//
uint32_t Send_Message_A1 (void)
{
	uint32_t errorcode; //��� ������ CAN
	uint8_t *msg_flags;
	uint8_t CAN_Tx_buffer[8];

	//������������ CAN - ���������
	CAN_TxHeader.StdId = MAKE_FRAME_ID(MSG_TYPE_A1, (uint8_t)MKS2.canContext.Addr); //ID ��������� ��������� �2
	CAN_TxHeader.ExtId = 0;
	CAN_TxHeader.RTR = CAN_RTR_DATA; //��� ��������� (CAN_RTR_Data - �������� ������)
	CAN_TxHeader.IDE = CAN_ID_STD;   //������ ����� Standard
	CAN_TxHeader.DLC = 0x8; //���������� ���� � ���������
	CAN_TxHeader.TransmitGlobalTime = 0;
	
	MESSAGE_A1.module_type = MKS2.canContext.ID;
	MESSAGE_A1.data_type = 0x01; //��� ��������� �1
	
	MESSAGE_A1.Type3.time2k = MKS2.tmContext.Time2k; //���������� ������ � 01.01.2000
	
	MESSAGE_A1.Type3.ls_tai = MKS2.tmContext.TAI_UTC_offset;  //������� ����� ������� �������� � �������� UTC
	

	MESSAGE_A1.Type3.ls_59 = MKS2.tmContext.LeapS_59; //1-��������� ������ ����� �������� 59 ������
	MESSAGE_A1.Type3.ls_61 = MKS2.tmContext.LeapS_61; //1-��������� ������ ����� �������� 61 �������
	MESSAGE_A1.Type3.moscow_tz = 0; //�������� ����� ������� � ��������� �������� � �����
	MESSAGE_A1.Type3.local_tz = -128; //�������� ����� ������� � ����������� �������� � ��������� ����
		
	memcpy( CAN_Tx_buffer, &MESSAGE_A1.RAW[0], 8); 	
	
	CAN1_Send_Message (&CAN_TxHeader, CAN_Tx_buffer);
		
	return errorcode;
}

//------------------------------------------------------------------------------------------------------------//
uint32_t CAN1_Send_Message (CAN_TxHeaderTypeDef * TxHeader, uint8_t * CAN_TxData)
{
	uint32_t errorcode; //��� ������ CAN
	uint32_t uwCounter = 0;
	
	
	while ((HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0) && (uwCounter != 0xFFFF)) //�������� ������������ TxMailbox
		{uwCounter++;} 
	
	if (uwCounter == 0xFFFF)	//����� �� ����-����
		{return (errorcode = HAL_TIMEOUT);}
	
	if (READ_BIT (CAN1->TSR, CAN_TSR_TME0)) 
		{TxMailbox = 0;}
	else
	{
		if (READ_BIT (CAN1->TSR, CAN_TSR_TME1)) 
			{TxMailbox = 1;}
		else
		{
			if (READ_BIT (CAN1->TSR, CAN_TSR_TME2)) 
				{TxMailbox = 2;}
		}
	}
	return (errorcode = HAL_CAN_AddTxMessage(&hcan, TxHeader, CAN_TxData, &TxMailbox)); //���������� ��������� � ������ ��������� Mailboxe Tx � ��������� ������� �� ��������  
}
/* USER CODE END 1 */
