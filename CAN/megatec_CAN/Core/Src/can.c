// Includes -------------------------------------------------------------------------------------------//
#include "can.h"
#include <stdio.h>
#include <stdbool.h>
#include "usart.h"
#include "pins.h"
#include "lib_delay.h"
#include "HW_Profile.h"

// Private functions prototypes -----------------------------------------------------------------------//
static TRxResult ReadMsgCAN(void);
static uint32_t Send_Message_C2 (void);
static uint32_t CAN1_Send_Message (CAN_TxHeaderTypeDef * , uint8_t * );
static void Task_ProcCANRequests_And_CheckCANCondition( void );

//Private variables -----------------------------------------------------------------------------------//
static CAN_TxHeaderTypeDef CAN_TxHeader; //��������� ��� �������� ������ CAN1 
static CAN_RxHeaderTypeDef CAN_RxHeader; //��������� ��� ����� ������ CAN1

static CAN_RX_msg CAN1_RX; //��������� ��� �������� ������ CAN1
static CAN_MSG_TYPE_C_MKIP my_can_msg = {0, 0}; //���������� ��������� ��� �������� ������ ��������� ���� �2
	
static uint32_t ID_C2 = 0; //CAN ��������� ��������� ���� �2
static uint32_t MKIPModuleAddress = 0;	// ����� � ������

CAN_HandleTypeDef hcan;

//-----------------------------------------------------------------------------------------------------//
void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
   // __HAL_RCC_CAN1_CLK_ENABLE(); //CAN1 clock enable 
		SET_BIT(RCC->APB1ENR, RCC_APB1ENR_CAN1EN);

    __HAL_RCC_GPIOA_CLK_ENABLE();
     
    GPIO_InitStruct.Pin = CAN_RX_PIN; //PA11--> CAN_RX 
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(CAN_RX_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CAN_TX_PIN; //PA12--> CAN_TX  
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(CAN_TX_PORT, &GPIO_InitStruct);

		//����� ����� GPIO ��� CAN1
		if (CAN_TX_PORT == GPIOA)
			{__HAL_AFIO_REMAP_CAN1_1();} //CAN_RX mapped to PA11, CAN_TX mapped to PA12
		else
			if (CAN_TX_PORT == GPIOB)
				{__HAL_AFIO_REMAP_CAN1_2();} //CAN_RX mapped to PB8,  CAN_TX mapped to PB9
			else
					if (CAN_TX_PORT == GPIOD)
						{__HAL_AFIO_REMAP_CAN1_3();} //CAN_RX mapped to PD0,  CAN_TX mapped to PD1
					
    // CAN1 interrupt Init //
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn); //��������� ��������� �� ����� � ������ FIFO0
    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn); //��������� ��������� ��� ������  
  }
}
//-----------------------------------------------------------------------------------------------------//
void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
    __HAL_RCC_CAN1_CLK_DISABLE();  // Peripheral clock disable /

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12); //PA11-> CAN_RX , PA12-> CAN_TX

    // CAN1 interrupt Deinit //
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
  }
}


//-----------------------------------------------------------------------------------------------------//
void init_CAN (void)
{
	hcan.Instance = MY_CAN;
  hcan.Init.Prescaler = 18; //������������ CAN
  hcan.Init.Mode = CAN_MODE_NORMAL; //����� ������ CAN
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE; //���� ��������, ����� ���� ������������ � Time Master � � ����������� ���������� �������� �������� � ���� ���������, �� ������� ������ ���� ����������������
  hcan.Init.AutoBusOff = ENABLE; //���� Automatic Bus-Off �������, �� CAN, ����� ������������� �����������������
  hcan.Init.AutoWakeUp = ENABLE; //���� ��������, �� ���������� �� ���� �������� ������ ����
  hcan.Init.AutoRetransmission = DISABLE; //��� ���������, ���� ����� ��������� ������� ��������� ��������� ���� �� �������� ������������� �����
  hcan.Init.ReceiveFifoLocked = DISABLE; //���� ��������, ����� ���� ��� mailbox FIFO ���������, � ��������� �� ������������, ��������� ��������� ����� ���������������� �����
  hcan.Init.TransmitFifoPriority = ENABLE; //���� ����� �������, ����� ��������� ������������ �� mailbox �� �������� FIFO � ������ ������, ������ �����. ���� ��������, ����� ������� ������� ��������� � ����� ������� �����������
  
	if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }

	MKIPModuleAddress = Get_Module_Address(); //��������� ������ � �����-�����
	ID_C2 = MAKE_FRAME_ID(MSG_TYPE_C, (uint8_t)MKIPModuleAddress); //������������ � ���������� ID CAN-���������
	
	//---------------------------------------��������� ������� ��� FIFO0--------------------------------------//
	CAN_FilterTypeDef can_FIFO_filter;
	
	#ifdef __USE_DBG
		sprintf (buffer_TX_UART3, (char *)"FRAME_ID=%x\r\n", ((MAKE_FRAME_ID(MSG_TYPE_C, (uint8_t)ID_C2))<<5));
		UART3_PutString (buffer_TX_UART3);
	#endif
	
	can_FIFO_filter.FilterBank = 0; //����� �������
	can_FIFO_filter.FilterIdHigh =(ID_C2<<5); // ������� ����� ������� �������� �������
	can_FIFO_filter.FilterIdLow = 0x0000; //������� ����� ������� �������� �������
	can_FIFO_filter.FilterMaskIdHigh = (0x7FF << 5); //������� ����� ����� ������� ����� 0b11111111111 (���� ������ ��������� ���� �)
	can_FIFO_filter.FilterMaskIdLow = 0x0000; //������� ����� ����� �������
	can_FIFO_filter.FilterFIFOAssignment = CAN_RX_FIFO0; //��������� ������� ��� �������� ������ CAN_RX_FIFO0
	can_FIFO_filter.FilterMode = CAN_FILTERMODE_IDMASK; //����� ������ �������
	can_FIFO_filter.FilterScale =  CAN_FILTERSCALE_32BIT; //����������� �������, 32 ���� - ������������� ����� ���� ����������� (11 ���) ��������������, ���� ����������� (29 ���)
	can_FIFO_filter.FilterActivation = ENABLE;
	
	if(HAL_CAN_ConfigFilter(&hcan, &can_FIFO_filter) != HAL_OK)
		{Error_Handler();}
		
	//---------------------------------------��������� ������� ��� FIFO1--------------------------------------//
/*	can_FIFO_filter.FilterBank = 1; //����� �������
	can_FIFO_filter.FilterIdHigh = ((MAKE_FRAME_ID(MSG_TYPE_A1, (uint8_t)MyModuleAddress))<<5); // ������� ����� ������� �������� �������
	can_FIFO_filter.FilterIdLow = 0x0000; //������� ����� ������� �������� �������
	can_FIFO_filter.FilterMaskIdHigh = (0x7FF << 5); // ������� ����� ����� �������
	can_FIFO_filter.FilterMaskIdLow = 0x0000; //������� ����� ����� �������
	can_FIFO_filter.FilterFIFOAssignment = CAN_RX_FIFO1; //��������� ������� ��� �������� ������ CAN_RX_FIFO1
	can_FIFO_filter.FilterActivation = ENABLE;
	
	if(HAL_CAN_ConfigFilter(&hcan, &can_FIFO_filter) != HAL_OK)
		{Error_Handler();}*/
		
	HAL_CAN_Start(&hcan); 
		
	//��������� ���������� CAN: CAN_IT_RX_FIFO0_MSG_PENDING - ���������� ��� ��������� ��������� � FIFO0 (���������� FIFO1);
	//CAN_IT_ERROR -	���������� ����� �������������, ����� � CAN_ESR ��������� ������� ������;
	//CAN_IT_BUSOFF -	���������� ����� ������������� ��� ��������� ���� BOFF;
	//CAN_IT_LAST_ERROR_CODE - ���������� ����� �������������, ����� ��� ������ ����� ���������� � ����� LEC[2:0];
	//CAN_IT_ERROR_PASSIVE - ����������, ����� ��������� ������ ��������� ������ (������� ������ ������ ��� ������� ������ ��������>127)
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING 
	| CAN_IT_ERROR | CAN_IT_BUSOFF | CAN_IT_LAST_ERROR_CODE | CAN_IT_ERROR_PASSIVE); 
	
	CAN1_RX.flag_RX = RX_NONE; //��������� ������� ����� CAN: ��������� �� �������
}

//-------------------------------------------------------------------------------------------------------------//
void CAN_Reinit (void)
{
	#ifdef __USE_DBG
		UART3_PutString ("can_reinit\r\n");
	#endif
	HAL_CAN_Stop (&hcan); //��������� CAN
	HAL_CAN_DeInit (&hcan); //����� ���������
	init_CAN (); //������������� CAN
}


//--------------------------------------������� ��� ������ ����� FIFO �0--------------------------------------//
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) 
{
	if(HAL_CAN_GetRxMessage (hcan, CAN_RX_FIFO0, &CAN_RxHeader, CAN1_RX.RxData) == HAL_OK) //���� ������ ���������� ��������� ������ � ����� FIFO0 CAN1
	{
		CAN1_RX.flag_RX = RX_UNKNOWN; //��������� ������� ����� CAN: ������� ��������������������� ��������� 
		#ifdef __USE_DBG
			sprintf (buffer_TX_UART3, (char *)"FIFO0_id=%x,msg=%x_%x,my_id=%x\r\n", CAN_RxHeader.StdId, CAN1_RX.RxData[0], CAN1_RX.RxData[1], ID_C2);
			UART3_PutString (buffer_TX_UART3);
		#endif
	}	
}

//---------------------------------������� ������ �� ������������ Fifo0---------------------------------//
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan)
{
	g_MyFlags.CAN_Fail = CAN_ERROR;
	#ifdef __USE_DBG
		sprintf (buffer_TX_UART3, (char *)"CAN_FIFO0_Full");
		UART3_PutString (buffer_TX_UART3);
	#endif
}

//----------------------------------������� ��� ������ ����� FIFO �1----------------------------------//
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) 
{
	if(HAL_CAN_GetRxMessage (hcan, CAN_RX_FIFO1, &CAN_RxHeader, CAN1_RX.RxData) == HAL_OK) //���� ������ ���������� ��������� ������ � ����� FIFO0 CAN1
	{
		CAN1_RX.flag_RX = RX_UNKNOWN; //��������� ������� ����� CAN: ������� ��������������������� ��������� 
		#ifdef __USE_DBG
			sprintf (buffer_TX_UART3, (char *)"FIFO1_id=%x, msg=%x_%x\r\n", CAN_RxHeader.StdId, CAN1_RX.RxData[0], CAN1_RX.RxData[1]);
			UART3_PutString (buffer_TX_UART3);
		#endif
	}	
}

//---------------------------------������� ������ �� ������������ Fifo1---------------------------------//
void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan)
{
	g_MyFlags.CAN_Fail = CAN_ERROR;
	#ifdef __USE_DBG
		sprintf (buffer_TX_UART3, (char *)"CAN_FIFO1_Full");
		UART3_PutString (buffer_TX_UART3);
	#endif
}

//------------------------------------------������� ������ CAN------------------------------------------//
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	uint32_t errorcode = 0;
	if ((errorcode = HAL_CAN_GetError(hcan)) != HAL_OK)
	{
		g_MyFlags.CAN_Fail = CAN_ERROR;
		#ifdef __USE_DBG
			sprintf (buffer_TX_UART3, (char *)"CAN1_ERROR=%u\r\n", errorcode);
			UART3_PutString (buffer_TX_UART3);
		#endif
		if ((errorcode == HAL_CAN_ERROR_EPV ) || (errorcode == HAL_CAN_ERROR_BOF )) //���� ������� ������ CAN > 128 ��� ���������� ���� BUSOFF
		{
			CAN_Reinit(); //������������ 	CAN		
		}
	}
}	

//--------------------------------------------������ CAN ���������--------------------------------------------//
static TRxResult ReadMsgCAN(void)
{
	if (CAN1_RX.flag_RX == RX_NONE) //����  ��������� �� ���� ��������
		{return CAN1_RX.flag_RX;}

	if ((CAN_RxHeader.RTR == CAN_RTR_REMOTE) && (CAN_RxHeader.DLC == 0)) //���� � �������� ��������� ���������� ��� RTR � ��� ������ (�1)
	{
		if ((CAN_RxHeader.StdId & MKIPModuleAddress) == MKIPModuleAddress) //���� � ��������� ����� ����� MKIP ���������� �����
			{return (CAN1_RX.flag_RX = RX_C1);} //���� ��������� ��������� �1
	}
	else
	{
		// ���� �� ���������� RTR - ��������, ��� ��� ���� ����������� ����������� ��������� �2
		if( (CAN_RxHeader.DLC == 8) && ( CAN_RxHeader.RTR == CAN_RTR_DATA) && ((CAN_RxHeader.StdId & MKIPModuleAddress) == MKIPModuleAddress))
			{return (CAN1_RX.flag_RX = RX_OWN_C2);} //���� ���������  ��������� ��������� �2
	}
	return CAN1_RX.flag_RX;
}

//-----------------------------------------------------------------------------------------------------//
void TaskCAN( void )
{
	Task_ProcCANRequests_And_CheckCANCondition();
}

//-----------------------------------------------------------------------------------------------------//
static void Task_ProcCANRequests_And_CheckCANCondition( void )
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
   	
	switch	(	ReadMsgCAN() )
	{
		case RX_C1: //������� ������ C1  

			if ((errorcode = Send_Message_C2()) != HAL_OK ) //����������� ��������� C2
				{g_MyFlags.CAN_Fail = CAN_ERROR;} //���� �������� �� ������� - ��������� ����� ������ CAN
			else
				{g_MyFlags.CAN_Fail = CAN_OK;}  // ����� ����� ������ CAN
			CAN1_RX.flag_RX = RX_NONE; //����� ����� ����������� CAN ���������
			last_c2_tx_ticks = current_ticks; //����������� ������� �������� ��������� �2
			break;

		case RX_OWN_C2: //�������� ����������� ��������� C2 

			g_MyFlags.CAN_Fail = CAN_OK;  // ����� ����� ������ CAN
			//last_c2_rx_ticks = current_ticks; //���������� �������� ���������� �����
			CAN1_RX.flag_RX = RX_NONE; //����� ����� ����������� CAN ���������
			break;

		case RX_NONE:
		break;
		
		case RX_UNKNOWN:
			CAN1_RX.flag_RX = RX_NONE; //����� ����� ����������� CAN ���������
			break;
		
		default:
		break;
	}
	
	if( current_ticks - last_c2_tx_ticks > 4*TICKS_PER_SECOND ) //���� ����� (4�) �� ���������� �2 (� ����� �� �1)
	{		
		if ((errorcode = Send_Message_C2 ()) != HAL_OK ) //�������� ��������� C2 ��� �������� ����������������� CAN, ��������� ������� ��������
		{
			g_MyFlags.CAN_Fail = CAN_ERROR; // ��������� ����� ������ CAN
			CAN_Reinit ();	
		} 
		last_c2_tx_ticks = current_ticks; //���������� �������� ���������� �����
	}
}

//--------------------------------------------�������� ��������� �2--------------------------------------------//
static uint32_t Send_Message_C2 (void)
{
	uint32_t errorcode; //��� ������ CAN
	
	//my_can_msg = {0, 0};
	my_can_msg.data_type = 0; //������� 3 ���� 1 ����� ��������� �2 ����� 0
	my_can_msg.module_type = MODULE_TYPE_MKIP; //������ � ������ ���� ��������� ���� ������-����������� - ���� (0�15)
	my_can_msg.state = g_MyFlags.UPS_state; //������ �� ������ ���� ������� UPSa
	
	//������������ CAN - ���������
	CAN_TxHeader.StdId = ID_C2; //ID - ����������� ��������� 
	CAN_TxHeader.ExtId = 0;
	CAN_TxHeader.RTR = CAN_RTR_DATA; //��� ��������� (CAN_RTR_Data - �������� ������)
	CAN_TxHeader.IDE = CAN_ID_STD;   //������ ����� Standard
	CAN_TxHeader.DLC = 8; //���������� ���� � ���������
	CAN_TxHeader.TransmitGlobalTime = 0;
	
	return (errorcode = CAN1_Send_Message (&CAN_TxHeader, my_can_msg.bytes)); //�������� ��������� �2
}

//---------------------------------------�-� �������� ��������� �� CAN1---------------------------------------//
static uint32_t CAN1_Send_Message (CAN_TxHeaderTypeDef * TxHeader, uint8_t * CAN_TxData)
{
	uint32_t errorcode; //��� ������ CAN
	uint32_t uwCounter = 0; 
	uint32_t TxMailbox = 0;//����� ��������� ����� ��� �������� 
	
	while ((HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0) && (uwCounter != 0xFFFF)) //�������� ������������ TxMailbox
		{uwCounter++;} 
	
	if (uwCounter == 0xFFFF)	//���� TxMailbox �� ������������
		{return (errorcode = HAL_TIMEOUT);} //����� �� ����-����
	
	if (READ_BIT (CAN1->TSR, CAN_TSR_TME0)) //�������� ���� ���������� TxMailbox �0
		{TxMailbox = 0;}
	else
	{
		if (READ_BIT (CAN1->TSR, CAN_TSR_TME1)) //�������� ���� ���������� TxMailbox �1
			{TxMailbox = 1;}
		else
		{
			if (READ_BIT (CAN1->TSR, CAN_TSR_TME2)) //�������� ���� ���������� TxMailbox �2
				{TxMailbox = 2;}
			else
				{return (errorcode = HAL_BUSY);} 
		}
	}
	
	//���������� ��������� � ��������� Mailbox � ��������� ������� �� ��������  
	return (errorcode = HAL_CAN_AddTxMessage(&hcan, TxHeader, CAN_TxData, &TxMailbox)); 
}

//-------------------------------------------------------------------------------------------------------------//

