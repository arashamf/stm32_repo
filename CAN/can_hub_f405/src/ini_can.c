/*
 * ini_can.c
 *
 *  Created on: 18 мар. 2021 г.
 *      Author: Пользователь
 */
#include "stm32f4xx.h" // описание периферии
#include "ini_can.h"

uint8_t transmit_mailbox = 0; //номер передающего ящика (от 0 до 2)
uint32_t uwCounter = 0; //счётчик
uint8_t status_Tx = 0;

/*----------------------------------------------------------------------------------------------------------*/
	void init_GPIO_CAN1 ()
	{

		RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //затактируем порт A

		GPIO_InitTypeDef GPIO_CAN_init;

		// Настраиваем CAN1 RX pin - PA11
		GPIO_CAN_init.GPIO_Pin =  GPIO_Pin_11;
		GPIO_CAN_init.GPIO_Mode  = GPIO_Mode_AF; //альтернативная ф-я
		GPIO_CAN_init.GPIO_OType = GPIO_OType_PP; //включим push-pull
//		GPIO_CAN_init.GPIO_PuPd = GPIO_PuPd_UP; //подтяжка к питанию
		GPIO_CAN_init.GPIO_Speed = GPIO_High_Speed; //скорость
		GPIO_Init (GPIOA, &GPIO_CAN_init);

		// Настраиваем CAN1 TX pin - PA12
		GPIO_CAN_init.GPIO_Pin =  GPIO_Pin_12;
		GPIO_CAN_init.GPIO_Mode  = GPIO_Mode_AF; //альтернативная ф-я
		GPIO_CAN_init.GPIO_OType = GPIO_OType_PP; //включим push-pull
//		GPIO_CAN_init.GPIO_PuPd = GPIO_PuPd_UP; //подтяжка к питанию
		GPIO_CAN_init.GPIO_Speed = GPIO_High_Speed; //скорость
		GPIO_Init(GPIOA, &GPIO_CAN_init);

		GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1); //CAN_RX =  PA11
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1); //CAN_TX = PA12

	}

	/*----------------------------------------------------------------------------------------------------------*/
	void init_CAN1 ()
	{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); // включаем тактирование шины CAN1

	CAN_DeInit(CAN1); //сброс всех настроек

	init_GPIO_CAN1 ();

	CAN_InitTypeDef CAN_InitStruct;
	CAN_InitStruct.CAN_TTCM = DISABLE; //включает внутренний 16-битный таймер, используемый для привязки метки времени к каждому принятому и отправленному сообщению. Этот таймер начинает счет с момента разрешения работы контроллера CAN.
	CAN_InitStruct.CAN_ABOM = ENABLE; //Если режим включен, то МК автоматически начнет проверять сигнал CAN_RX для восстановления шины
	CAN_InitStruct.CAN_AWUM = DISABLE; //При включении этого параметра устройство будет автоматически просыпаться (первый пакет, переданный по шине, может быть утерян).
	CAN_InitStruct.CAN_NART = DISABLE; //Режим проверки получения пакета. Если этот режим включен, то МК при передаче кадра не будет проверять подтверждение получения пакета на шине. Если выключен - то, при передаче кадра, МК будет слушать шину на предмет получения подтверждения от всех устройства о том, что пакет получен и, если не получен хотя бы 1 устройством, то будет пытаться отправить повторно до тех пор, пока все устройства не подтвердят получение пакета.
	CAN_InitStruct.CAN_RFLM = DISABLE; //Режим блокировки Receive FIFO. 0 - при переполнении RX_FIF0 прием не прерывается , каждый новый пакет затирает предыдущий
	CAN_InitStruct.CAN_TXFP = ENABLE; //Порядок отправки сообщений в шину. Если параметр включен, то сообщения отправляются в хронологическом порядке: FIFO. Если же выключен, то пакеты передаются в зависимости от приоритета ID пакета (пакет с более высоким приоритетом будет отправлен раньше).

	/*-------------------------------baudrate = 25 кБит/с при 20MHz----------------------------*/
//	CAN_InitStruct.CAN_Mode = CAN_Mode_Silent; //мк только слушает шину
//	CAN_InitStruct.CAN_Mode = CAN_Mode_LoopBack; //При выборе режима LoopBack, мк будет передавать данные в шину и слушать себя же одновременно, пакеты из шины доходить до контроллера не будут.
//	CAN_InitStruct.CAN_Mode = CAN_Mode_Silent_LoopBack; //В данном режиме все пакеты будут полностью крутится внутри контроллера без выхода в общую шину.
	CAN_InitStruct.CAN_Mode = CAN_Mode_Normal; //При этом параметре МК будет работать в нормальном режиме. Данные будут передаваться и читаться из шины.
	CAN_InitStruct.CAN_SJW = CAN_SJW_1tq; //SJW определяет макс. количество квантов времени, на которое может быть увеличено или уменьшено количество квантов времени битовых сегментов. Возможные значения этого показателя от 1-го до 4-х квантов.
	CAN_InitStruct.CAN_BS1 = CAN_BS1_13tq; //BS1 - определяет местоположение точки захвата (Sample Point). Он включает в себя Prop_Seg и PHASE_SEG1 стандарта CAN. Его продолжительность программируется от 1 до 16 квантов
	CAN_InitStruct.CAN_BS2 = CAN_BS2_2tq; //BS2 - определяет местоположение точки передачи. Он представляет собой PHASE_SEG2 стандарта CAN. Его продолжительность программируется от 1 до 8 квантов времени
	CAN_InitStruct.CAN_Prescaler = 90; //предделитель САN

	CAN_Init (CAN1, &CAN_InitStruct);

	// Настройка фильтрации пакетов
	CAN_FilterInitTypeDef ini_CAN_Filter;
	ini_CAN_Filter.CAN_FilterNumber = 0;
	ini_CAN_Filter.CAN_FilterMode = CAN_FilterMode_IdMask;
	ini_CAN_Filter.CAN_FilterScale = CAN_FilterScale_32bit;
	ini_CAN_Filter.CAN_FilterIdHigh = 0x0000;
	ini_CAN_Filter.CAN_FilterIdLow = 0x0000;
	ini_CAN_Filter.CAN_FilterMaskIdHigh = 0x0000;
	ini_CAN_Filter.CAN_FilterMaskIdLow = 0x0000;
	ini_CAN_Filter.CAN_FilterFIFOAssignment = CAN_FIFO0;
	ini_CAN_Filter.CAN_FilterActivation = ENABLE;
	CAN_FilterInit (&ini_CAN_Filter);

	NVIC_InitTypeDef NVIC_InitStruct1;
	NVIC_InitStruct1.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStruct1.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct1.NVIC_IRQChannelSubPriority = 0x1;
	NVIC_InitStruct1.NVIC_IRQChannelCmd = ENABLE; //вкл. прерывания от CAN1_RX0
	NVIC_Init (&NVIC_InitStruct1);

	CAN_ITConfig (CAN1, CAN_IT_FMP0, ENABLE); // разрешение прерывания по приему сообщения в FIFO_0 CAN1
	CAN_ITConfig(CAN1, CAN_IT_FF0, ENABLE); // Прерывание при заполнении буфера FIFO 0
	CAN_ITConfig(CAN1, CAN_IT_FOV0, ENABLE); // Прерывание при переполнении буфера FIFO 0

/*	NVIC_InitTypeDef NVIC_InitStruct2;
	NVIC_InitStruct2.NVIC_IRQChannel = CAN1_RX1_IRQn;
	NVIC_InitStruct2.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct2.NVIC_IRQChannelSubPriority = 0x2;
	NVIC_InitStruct2.NVIC_IRQChannelCmd = ENABLE; //вкл. прерывания от CAN1_RX1
	NVIC_Init (&NVIC_InitStruct2);

	CAN_ITConfig (CAN1, CAN_IT_FMP1, ENABLE); // разрешение прерывания по приему сообщения в FIFO_0 CAN1

	NVIC_InitTypeDef NVIC_InitStruct3;
	NVIC_InitStruct3.NVIC_IRQChannel = CAN1_TX_IRQn;
	NVIC_InitStruct3.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct3.NVIC_IRQChannelSubPriority = 0x3;
	NVIC_InitStruct3.NVIC_IRQChannelCmd = ENABLE; //вкл. прерывания от CAN1_TX
	NVIC_Init (&NVIC_InitStruct3);

	CAN_ITConfig (CAN1, CAN_IT_TME, ENABLE); //разрешение прерывания по передаче TX*/

	//прерывание изменения статуса CAN1
	NVIC_InitTypeDef NVIC_InitStructure4;
	NVIC_InitStructure4.NVIC_IRQChannel = CAN1_SCE_IRQn;
	NVIC_InitStructure4.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure4.NVIC_IRQChannelSubPriority = 0x4;
	NVIC_InitStructure4.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure4);

	// Обрабатывается в прерывании CAN1_SCE_IRQHandler
	CAN_ITConfig(CAN1, CAN_IT_WKU, ENABLE);         // Прерывание при "пробуждении" - выход из "спящего" режима
	CAN_ITConfig(CAN1, CAN_IT_SLK, ENABLE);         // Прерывание при переходе в "спящий" режим
	CAN_ITConfig(CAN1, CAN_IT_EWG, ENABLE);         // Error warning Interrupt (error counter >= 96)
	CAN_ITConfig(CAN1, CAN_IT_EPV, ENABLE);         // Error passive Interrupt (error counter > 127)
	CAN_ITConfig(CAN1, CAN_IT_BOF, ENABLE);         // Bus-off Interrupt (error counter > 255)
	CAN_ITConfig(CAN1, CAN_IT_LEC, ENABLE);         // Last error code - при возникновении ошибок приема-передачи
	CAN_ITConfig(CAN1, CAN_IT_ERR, ENABLE);         // Прерывание при возникновении ошибок bxCan
	}

	/*----------------------------------------отправка тестового сообщения в шину CAN1----------------------------------------*/
	uint8_t CAN1_Send_Test()
	{
		CanTxMsg TxMessage;
		TxMessage.StdId = CAN_CMD_Test_Send; //идентификатор стандартного кадра
		TxMessage.ExtId = 0x00; // идентификатор расширенного кадра. Если отправляется стандартный, сюда надо писать 0
		TxMessage.IDE = CAN_Id_Standard; // формат кадра: CAN_Id_Standard или CAN_Id_Extended
		TxMessage.RTR = CAN_RTR_DATA; // тип сообщения (CAN_RTR_Data - передача данных)
		TxMessage.DLC = 5; // Длина блока данных (max 8 байт)

		TxMessage.Data[0] = 0xCA; // Байт данных №1
		TxMessage.Data[1] = 0xFE; // Байт данных №2
		TxMessage.Data[2] = 0xDE; // Байт данных №3
		TxMessage.Data[3] = 0xAD; // Байт данных №4
		TxMessage.Data[4] = 0xCA; // Байт данных №5
		TxMessage.Data[5] = 0xFE; // Байт данных №6
		TxMessage.Data[6] = 0xDE; // Байт данных №7
		TxMessage.Data[7] = 0xAD; // Байт данных №8

		transmit_mailbox = CAN_Transmit(CAN1, &TxMessage);
		uwCounter = 0;
		while ((status_Tx = (CAN_TransmitStatus(CAN1, transmit_mailbox))  !=  CAN_TxStatus_Ok) && (uwCounter  !=  0xFFFF))
		{
			uwCounter++; //ждём установки флага CAN_TSR_RQCPх, CAN_TSR_TXOKx, CAN_TSR_TMEx или выходим по тайм-ауту
		}
		return status_Tx;
	}

/*-----------------------------------отправка подтверждения получения тестовой команды в шину CAN1--------------------------------------*/
	void CAN1_Send_Ok ( )
	{
		CanTxMsg TxMessage;
		TxMessage.StdId = CAN_CMD_Test_Ok;	// команда шины
		TxMessage.ExtId = 0x00;				// заголовок расширенной команды не указываем
		TxMessage.IDE = CAN_Id_Standard;	// формат кадра Standard
		TxMessage.RTR = CAN_RTR_DATA;		// тип сообщения (CAN_RTR_Data - передача данных)
		TxMessage.DLC = 0;					// Длина блока данных 0 - ничего не передаем

		CAN_Transmit(CAN1, &TxMessage);
	}

	//---------------------------------------------ф-я отправки сообщения на шину CAN1---------------------------------------------------//
	uint8_t CAN1_TxMessage (CanTxMsg * TxMessage)
	{
		transmit_mailbox = CAN_Transmit(CAN1, TxMessage); //передадим сообщение и получим номер ящика
		uwCounter = 0;
		while ((status_Tx = (CAN_TransmitStatus(CAN1, transmit_mailbox))  !=  CAN_TxStatus_Ok) && (uwCounter  !=  0xFFFF))
			{
			uwCounter++; //ждём установки флага CAN_TSR_RQCPх, CAN_TSR_TXOKx, CAN_TSR_TMEx или выходим по тайм-ауту
			}
		return status_Tx; //если передача успешна вернём 1
	}

/*----------------------------------------------------------------------------------------------------------*/
void init_GPIO_CAN2 ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB, ENABLE); //затактируем порт B

	GPIO_InitTypeDef GPIO_CAN_init;

	// Настраиваем CAN RX pin - PB12
	GPIO_CAN_init.GPIO_Pin =  GPIO_Pin_12;
	GPIO_CAN_init.GPIO_Mode  = GPIO_Mode_AF; //альтернативная ф-я
	GPIO_CAN_init.GPIO_OType = GPIO_OType_PP; //включим push-pull
//	GPIO_CAN_init.GPIO_PuPd = GPIO_PuPd_UP; //подтяжка
	GPIO_CAN_init.GPIO_Speed = GPIO_High_Speed; //скорость средняя
	GPIO_Init (GPIOB, &GPIO_CAN_init);

	// Настраиваем CAN TX pin - PB13
	GPIO_CAN_init.GPIO_Pin =  GPIO_Pin_13;
	GPIO_CAN_init.GPIO_Mode  = GPIO_Mode_AF; //альтернативная ф-я
	GPIO_CAN_init.GPIO_OType = GPIO_OType_PP; //включим push-pull
//	GPIO_CAN_init.GPIO_PuPd = GPIO_PuPd_UP; //подтяжка
	GPIO_CAN_init.GPIO_Speed = GPIO_High_Speed; //скорость средняя
	GPIO_Init(GPIOB, &GPIO_CAN_init);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_CAN2); //CAN_RX =  PB12
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_CAN2); //CAN_TX = PB13
	}

/*----------------------------------------------------------------------------------------------------------*/
void init_CAN2 ()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);		// включаем тактирование шины CAN2

	CAN_DeInit(CAN2); //сброс всех настроек

	init_GPIO_CAN2 ();

	CAN_InitTypeDef CAN_InitStruct;
	CAN_InitStruct.CAN_TTCM = DISABLE; //включает внутренний 16-битный таймер, используемый для привязки метки времени к каждому принятому и отправленному сообщению. Этот таймер начинает счет с момента разрешения работы контроллера CAN.
	CAN_InitStruct.CAN_ABOM = ENABLE; //Если режим включен, то МК автоматически начнет проверять сигнал CAN_RX для восстановления шины
	CAN_InitStruct.CAN_AWUM = DISABLE; //При включении этого параметра устройство будет автоматически просыпаться (первый пакет, переданный по шине, может быть утерян).
	CAN_InitStruct.CAN_NART = DISABLE;; //Режим проверки получения пакета. Если этот режим включен, то МК при передаче кадра не будет проверять подтверждение получения пакета на шине. Если выключен - то, при передаче кадра, МК будет слушать шину на предмет получения подтверждения от всех устройства о том, что пакет получен и, если не получен хотя бы 1 устройством, то будет пытаться отправить повторно до тех пор, пока все устройства не подтвердят получение пакета.
	CAN_InitStruct.CAN_RFLM = DISABLE; //Режим блокировки Receive FIFO. 0 - при переполнении RX_FIF0 прием не прерывается , каждый новый пакет затирает предыдущий
	CAN_InitStruct.CAN_TXFP = ENABLE; //Порядок отправки сообщений в шину. Если параметр включен, то сообщения отправляются в хронологическом порядке: FIFO. Если же выключен, то пакеты передаются в зависимости от приоритета ID пакета (пакет с более высоким приоритетом будет отправлен раньше).

//	CAN_InitStruct.CAN_Mode = CAN_Mode_Silent; //мк только слушает шину
//	CAN_InitStruct.CAN_Mode = CAN_Mode_LoopBack; //При выборе режима LoopBack, мк будет передавать данные в шину и слушать себя же одновременно, пакеты из шины доходить до контроллера не будут.
//	CAN_InitStruct.CAN_Mode = CAN_Mode_Silent_LoopBack; //В данном режиме все пакеты будут полностью крутится внутри контроллера без выхода в общую шину.
	CAN_InitStruct.CAN_Mode = CAN_Mode_Normal; //При этом параметре МК будет работать в нормальном режиме. Данные будут передаваться и читаться из шины.
	CAN_InitStruct.CAN_SJW = CAN_SJW_1tq; //SJW определяет макс. количество квантов времени, на которое может быть увеличено или уменьшено количество квантов времени битовых сегментов. Возможные значения этого показателя от 1-го до 4-х квантов.
	CAN_InitStruct.CAN_BS1 = CAN_BS1_13tq; //BS1 - определяет местоположение точки захвата (Sample Point). Он включает в себя Prop_Seg и PHASE_SEG1 стандарта CAN. Его продолжительность программируется от 1 до 16 квантов
	CAN_InitStruct.CAN_BS2 = CAN_BS2_2tq; //BS2 - определяет местоположение точки передачи. Он представляет собой PHASE_SEG2 стандарта CAN. Его продолжительность программируется от 1 до 8 квантов времени
	CAN_InitStruct.CAN_Prescaler = 9; //предделитель САN

	CAN_Init (CAN2, &CAN_InitStruct);

//	Настройка фильтрации пакетов CAN Slave, у второго CAN фильтр по умолчанию №14
	CAN_FilterInitTypeDef ini_CAN_Filter;
	ini_CAN_Filter.CAN_FilterNumber = 14;
	ini_CAN_Filter.CAN_FilterMode = CAN_FilterMode_IdMask;
	ini_CAN_Filter.CAN_FilterScale = CAN_FilterScale_32bit;
	ini_CAN_Filter.CAN_FilterIdHigh = 0x0000;
	ini_CAN_Filter.CAN_FilterIdLow = 0x0000;
	ini_CAN_Filter.CAN_FilterMaskIdHigh = 0x0000;
	ini_CAN_Filter.CAN_FilterMaskIdLow = 0x0000;
	ini_CAN_Filter.CAN_FilterFIFOAssignment = CAN_FIFO0;
	ini_CAN_Filter.CAN_FilterActivation = ENABLE;
	CAN_FilterInit (&ini_CAN_Filter);

	// прерывание по приему в фифо 0
	NVIC_InitTypeDef NVIC_InitStruct1;
	NVIC_InitStruct1.NVIC_IRQChannel = CAN2_RX0_IRQn;
		NVIC_InitStruct1.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct1.NVIC_IRQChannelSubPriority = 0x1;
	NVIC_InitStruct1.NVIC_IRQChannelCmd = ENABLE; //вкл. прерывания от CAN2_RX0
	NVIC_Init (&NVIC_InitStruct1);

	CAN_ITConfig (CAN2, CAN_IT_FMP0, ENABLE); // разрешение прерывания по приему сообщения в FIFO_0 CAN2
	CAN_ITConfig(CAN2, CAN_IT_FF0, ENABLE); // Прерывание при заполнении буфера FIFO 0
	CAN_ITConfig(CAN2, CAN_IT_FOV0, ENABLE); // Прерывание при переполнении буфера FIFO 0

	// прерывание по приему в фифо 1
/*	NVIC_InitTypeDef NVIC_InitStruct2;
	NVIC_InitStruct2.NVIC_IRQChannel = CAN2_RX1_IRQn;
	NVIC_InitStruct2.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct2.NVIC_IRQChannelSubPriority = 0x2;
	NVIC_InitStruct2.NVIC_IRQChannelCmd = ENABLE; //вкл. прерывания от CAN2_RX0
	NVIC_Init (&NVIC_InitStruct2);

	CAN_ITConfig (CAN2, CAN_IT_FMP1, ENABLE); // разрешение прерывания по приему сообщения в FIFO_1 CAN2*/

	//прерывание по освобождению буфера передачи
	/*NVIC_InitTypeDef NVIC_InitStruct3;
	NVIC_InitStruct3.NVIC_IRQChannel = CAN2_TX_IRQn;
	NVIC_InitStruct3.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct3.NVIC_IRQChannelSubPriority = 0x3;
	NVIC_InitStruct3.NVIC_IRQChannelCmd = ENABLE; //вкл. прерывания от CAN2_TX
	NVIC_Init (&NVIC_InitStruct3);*/

	CAN_ITConfig (CAN2, CAN_IT_TME, ENABLE); //разрешение прерывания по передаче TX

	//прерывание изменения статуса CAN2
	NVIC_InitTypeDef NVIC_InitStructure4;
	NVIC_InitStructure4.NVIC_IRQChannel = CAN2_SCE_IRQn;
	NVIC_InitStructure4.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure4.NVIC_IRQChannelSubPriority = 0x4;
	NVIC_InitStructure4.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure4);

	// Обрабатывается в прерывании CAN2_SCE_IRQHandler
	CAN_ITConfig(CAN2, CAN_IT_WKU, ENABLE);         // Прерывание при "пробуждении" - выход из "спящего" режима
	CAN_ITConfig(CAN2, CAN_IT_SLK, ENABLE);         // Прерывание при переходе в "спящий" режим
	CAN_ITConfig(CAN2, CAN_IT_EWG, ENABLE);         // Error warning Interrupt (error counter >= 96)
	CAN_ITConfig(CAN2, CAN_IT_EPV, ENABLE);         // Error passive Interrupt (error counter > 127)
	CAN_ITConfig(CAN2, CAN_IT_BOF, ENABLE);         // Bus-off Interrupt (error counter > 255)
	CAN_ITConfig(CAN2, CAN_IT_LEC, ENABLE);         // Last error code - при возникновении ошибок приема-передачи
	CAN_ITConfig(CAN2, CAN_IT_ERR, ENABLE);         // Прерывание при возникновении ошибок bxCan
}

/*-----------------------------------отправка тестового сообщения в шину CAN2-----------------------------------*/
uint8_t CAN2_Send_Test()
{
	CanTxMsg TxMessage;
	TxMessage.StdId = CAN_CMD_Test_Send; //идентификатор стандартного кадра
	TxMessage.ExtId = 0x00; // идентификатор расширенного кадра. Если отправляется стандартный, сюда надо писать 0
	TxMessage.IDE = CAN_Id_Standard; // формат кадра: CAN_Id_Standard или CAN_Id_Extended
	TxMessage.RTR = CAN_RTR_DATA; // тип сообщения (CAN_RTR_Data - передача данных)
	TxMessage.DLC = 8; // Длина блока данных 3 байта (max 8 байт)

	TxMessage.Data[0] = 0x11; // Байт данных №1
	TxMessage.Data[1] = 0x12; // Байт данных №2
	TxMessage.Data[2] = 0x13; // Байт данных №3
	TxMessage.Data[3] = 0x14; // Байт данных №4
	TxMessage.Data[4] = 0x05; // Байт данных №5
	TxMessage.Data[5] = 0x16; // Байт данных №6
	TxMessage.Data[6] = 0x17; // Байт данных №7
	TxMessage.Data[7] = 0x18; // Байт данных №8


	transmit_mailbox = CAN_Transmit(CAN2, &TxMessage); //передадим сообщение
	uwCounter = 0;
	while ((CAN_TransmitStatus(CAN2, transmit_mailbox)  !=  CANTXOK) && (uwCounter  !=  0xFFFF))
	{
		uwCounter++; //ждём установки флага CANTXOK
	}
	return transmit_mailbox;
}

/*-----------------------------------отправка подтверждения получения тестовой команды в шину CAN2--------------------------------------*/
void CAN2_Send_Ok ( )
{
	CanTxMsg TxMessage;
	TxMessage.StdId = CAN_CMD_Test_Ok;	// команда шины
	TxMessage.ExtId = 0x00;				// Расширенную команду указывать нет смысла
	TxMessage.IDE = CAN_Id_Standard;	// формат кадра Standard
	TxMessage.RTR = CAN_RTR_DATA;		// тип сообщения (CAN_RTR_Data - передача данных)
	TxMessage.DLC = 0;					// Длина блока данных 0 - ничего не передаем

	CAN_Transmit(CAN2, &TxMessage);
}

//---------------------------------------------ф-я отправки сообщения на шину CAN2----------------------------------------------------//
	uint8_t CAN2_TxMessage (CanTxMsg * TxMessage)
	{
		transmit_mailbox = CAN_Transmit(CAN2, TxMessage); //передадим сообщение
		uwCounter = 0;
		while ((status_Tx = (CAN_TransmitStatus(CAN2, transmit_mailbox))  !=  CAN_TxStatus_Ok) && (uwCounter  !=  0xFFFF))
			{
			uwCounter++; //ждём установки флага CAN_TSR_RQCPх, CAN_TSR_TXOKx, CAN_TSR_TMEx или выходим по тайм-ауту
			}
		return status_Tx; //если передача успешна вернём 1
	}
