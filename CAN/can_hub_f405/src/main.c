/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f4xx.h"
#include "stdio.h"
#include "MCU_ini.h"
#include "delay_lib.h"
#include "UART_lib.h"
#include "ini_can.h"

#define BAUDRATE 57600 //пропускна¤ способность UART
#define DELAY 500 //величина задержки

#define msg_SIZE 50 //размер массива дл¤ формировани¤ сообщений дл¤ отправки по UART
char msg_UART_Tx [msg_SIZE]; // массив дл¤ формировани¤ сообщений дл¤ отправки по UART

#define LED_Pin GPIO_Pin_11 //номер пина светодиода
#define LED_GPIO_Port GPIOB //порт GPIO светодиода

uint8_t status_TX1 = 0; //статус передачи по шине CAN1
uint8_t status_TX2 = 0; //статус передачи по шине CAN2
uint8_t flag_CAN1_Rx = 0, flag_CAN2_Rx = 0; //флаги получения данных по шине CAN1 и CAN2

CanRxMsg Rx_buf1; //структура для приёма сообщения CAN1
CanRxMsg Rx_buf2; //структура для приёма сообщения CAN2
CanTxMsg TxMessage;

//========================================================================================//
void LED_ini ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB, ENABLE); //затактируем порт D

	GPIO_InitTypeDef 	GPIO_Init_LED;
	GPIO_Init_LED.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init_LED.GPIO_Mode = GPIO_Mode_OUT; //порт B 11 пин на обычный выход
	GPIO_Init_LED.GPIO_Speed = GPIO_Low_Speed; //скорость порта сама¤ низка¤
	GPIO_Init_LED.GPIO_OType = GPIO_OType_PP; //выход с подт¤жкой (push-pull)
	GPIO_Init_LED.GPIO_PuPd = GPIO_PuPd_NOPULL; //вход без подт¤жки

	GPIO_Init (GPIOB, &GPIO_Init_LED);
}

//========================================================================================//
void CAN1_RX0_IRQHandler ()
{
	GPIO_SetBits (LED_GPIO_Port, LED_Pin);

	if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) == SET)  // Прерывание получения пакета в буфер FIFO 0
	{
	Rx_buf1.DLC =     0x00; //
	Rx_buf1.ExtId =   0x00;
	Rx_buf1.FMI =     0x00;
	Rx_buf1.IDE =     0x00;
	Rx_buf1.RTR =     0x00;
	Rx_buf1.StdId =   0x00;

	CAN_Receive(CAN1, CAN_FIFO0, &Rx_buf1); //получаем сообщение
	UART1_PutString ("CAN1_get_FIFO0\r\n");

	TxMessage.DLC = Rx_buf1.DLC;
	TxMessage.ExtId = Rx_buf1.ExtId;
	TxMessage.IDE = Rx_buf1.IDE;
	TxMessage.RTR = Rx_buf1.RTR;
	TxMessage.StdId = Rx_buf1.StdId;
	TxMessage.Data [0] = Rx_buf1.Data [0];
	TxMessage.Data [1] = Rx_buf1.Data [1];
	TxMessage.Data [2] = Rx_buf1.Data [2];
	TxMessage.Data [3] = Rx_buf1.Data [3];
	TxMessage.Data [4] = Rx_buf1.Data [4];
	TxMessage.Data [5] = Rx_buf1.Data [5];
	TxMessage.Data [6] = Rx_buf1.Data [6];
	TxMessage.Data [7] = Rx_buf1.Data [7];

	status_TX2 = CAN2_TxMessage (&TxMessage); //передадим сообщение по CAN2
	flag_CAN1_Rx = 1; //установка флага получения сообщения по CAN1

	CAN_FIFORelease (CAN1, CAN_FIFO0); //очищает указанный регистр получения FIFO
	CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0); // Сбрасываем флаг прерывания
	}

	if (CAN_GetITStatus(CAN1, CAN_IT_FF0)==SET)  // Прерывание при заполнении буфера FIFO 0
	{
	CAN_ClearITPendingBit(CAN1, CAN_IT_FF0);
	UART1_PutString ("CAN1_FIFO0_full\r\n");
	CAN_ClearFlag(CAN1, CAN_FLAG_FF0); // после обработки сбросить флаг ошибки
	}

	if (CAN_GetITStatus(CAN1, CAN_IT_FOV0)==SET)  // Прерывание при переполнении буфера FIFO 0
	{
	CAN_ClearITPendingBit(CAN1, CAN_IT_FOV0);
	UART1_PutString ("CAN1_FIFO0_overflowing\r\n");
	CAN_ClearFlag(CAN1, CAN_FLAG_FOV0); // после обработки сбросить флаг ошибки
	}
}

//========================================================================================//
void CAN1_RX1_IRQHandler ()
{
	GPIO_SetBits (LED_GPIO_Port, LED_Pin);

	if (CAN_GetITStatus(CAN1, CAN_IT_FMP1) == SET)  // Прерывание получения пакета в буфер FIFO 1
	{
	CAN_Receive(CAN1, CAN_FIFO1, &Rx_buf1); //получаем сообщение
	UART1_PutString ("CAN1_get_FIFO1\r\n");

	TxMessage.DLC = Rx_buf1.DLC;
	TxMessage.ExtId = Rx_buf1.ExtId;
	TxMessage.IDE = Rx_buf1.IDE;
	TxMessage.RTR = Rx_buf1.RTR;
	TxMessage.StdId = Rx_buf1.StdId;
	TxMessage.Data [0] = Rx_buf1.Data [0];
	TxMessage.Data [1] = Rx_buf1.Data [1];
	TxMessage.Data [2] = Rx_buf1.Data [2];
	TxMessage.Data [3] = Rx_buf1.Data [3];
	TxMessage.Data [4] = Rx_buf1.Data [4];
	TxMessage.Data [5] = Rx_buf1.Data [5];
	TxMessage.Data [6] = Rx_buf1.Data [6];
	TxMessage.Data [7] = Rx_buf1.Data [7];

	status_TX2 = CAN2_TxMessage (&TxMessage); //передадим сообщение по CAN2
	flag_CAN1_Rx = 1; //установка флага получения сообщения по CAN1

	CAN_FIFORelease (CAN1, CAN_FIFO1); //очищает указанный регистр получения FIFO
	CAN_ClearITPendingBit(CAN1, CAN_IT_FMP1); // Сбрасываем флаг прерывания
	}
}


//========================================================================================//
void CAN1_TX_IRQHandler(void)
{

	if (CAN_GetITStatus(CAN1,CAN_IT_TME))
	{
		CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
		UART1_PutString ("CAN1_put_ok\r\n");
		GPIO_ResetBits (LED_GPIO_Port, LED_Pin);
	}
}

//=================================прерывание по изменению состояния CAN1=================================//
void CAN1_SCE_IRQHandler(void)
{

	uint8_t errorcode = 0;

if (CAN_GetITStatus(CAN1,CAN_IT_ERR)==SET) //устанавливается при возникновении любой ошибки шины
	{
	CAN_ClearITPendingBit(CAN1,CAN_IT_ERR);
	UART1_PutString ("CAN1_error\r\n");
	if (CAN_GetITStatus(CAN1,CAN_IT_EWG)==SET)  //предупреждение о том, что один из счетчиков ошибок достиг 96 или более ошибок
		{
		CAN_ClearITPendingBit(CAN1,CAN_IT_EWG);
		UART1_PutString ("CAN1_warning_error\r\n");
		}
	if (CAN_GetITStatus(CAN1,CAN_IT_EPV)==SET)  //предупреждение о том, что один из счетчиков ошибок достиг более 127 ошибок
		{
		CAN_ClearITPendingBit(CAN1,CAN_IT_EPV);
		UART1_PutString ("CAN1_passive_error\r\n");
		}
	if (CAN_GetITStatus(CAN1,CAN_IT_BOF)==SET)  //Возникает при переходе шины в режим Bus-Off, когда любой из счетчиков ошибок превысил значение 255
		{
		CAN_ClearITPendingBit(CAN1,CAN_IT_BOF);
		UART1_PutString ("CAN1_Bus-off_error\r\n");
		}
	if (CAN_GetITStatus(CAN1, CAN_IT_LEC)==SET) // Прерывание при ошибке приема передачи сообщения
		{
		CAN_ClearITPendingBit(CAN1, CAN_IT_LEC);
//		errorcode = CAN_GetLastErrorCode(CAN1); // Получим код ошибки
//		sprintf (msg_UART_Tx, "CAN1_transceiver_error, code = %x\r\n", errorcode);
//		UART1_PutString (msg_UART_Tx);
		UART1_PutString ("CAN1_transceiver_error\r\n");
		CAN_ClearFlag(CAN1, CAN_FLAG_LEC); //после обработки сбросить флаг ошибки
		}
	}
else
	{
	if (CAN_GetITStatus(CAN1, CAN_IT_WKU)==SET) // Прерывание при выходе из "спящего" режима
		{
		CAN_ClearITPendingBit(CAN1, CAN_IT_WKU);
		UART1_PutString ("CAN1_Wake_up\r\n");
		CAN_ClearFlag(CAN1, CAN_FLAG_WKU); // Не забываем после обработки сбросить флаг ошибки
		}
	if (CAN_GetITStatus(CAN1, CAN_IT_SLK)==SET) // Прерывание при переходе в "спящий" режим
		{
		CAN_ClearITPendingBit(CAN1, CAN_IT_SLK);
		UART1_PutString ("CAN1_Sleep\r\n");
		CAN_ClearFlag(CAN1, CAN_FLAG_SLAK); //после обработки сбросить флаг ошибки
		}
	}
}

//========================================================================================//
void CAN2_RX0_IRQHandler ()
{
	GPIO_SetBits (LED_GPIO_Port, LED_Pin);

	if (CAN_GetITStatus(CAN2, CAN_IT_FMP0) == SET)  // Прерывание получения пакета в буфер FIFO0
	{
	Rx_buf2.StdId =   0x00;
	Rx_buf2.ExtId =   0x00;
	Rx_buf2.IDE =     0x00;
	Rx_buf2.RTR =     0x00;
	Rx_buf2.DLC =     0x00;
	Rx_buf2.FMI =     0x00;
	CAN_Receive(CAN2, CAN_FIFO0, &Rx_buf2); //получаем сообщение
	UART1_PutString ("CAN2_get_FIFO0\r\n");

	TxMessage.DLC = Rx_buf2.DLC;
	TxMessage.ExtId = Rx_buf2.ExtId;
	TxMessage.IDE = Rx_buf2.IDE;
	TxMessage.RTR = Rx_buf2.RTR;
	TxMessage.StdId = Rx_buf2.StdId;
	TxMessage.Data [0] = Rx_buf2.Data [0];
	TxMessage.Data [1] = Rx_buf2.Data [1];
	TxMessage.Data [2] = Rx_buf2.Data [2];
	TxMessage.Data [3] = Rx_buf2.Data [3];
	TxMessage.Data [4] = Rx_buf2.Data [4];
	TxMessage.Data [5] = Rx_buf2.Data [5];
	TxMessage.Data [6] = Rx_buf2.Data [6];
	TxMessage.Data [7] = Rx_buf2.Data [7];

	status_TX1 = CAN1_TxMessage (&TxMessage); //передадим сообщение по CAN1
	flag_CAN2_Rx = 1; //установка флага получения сообщения по CAN2

	CAN_FIFORelease (CAN2, CAN_FIFO0); //очищает указанный регистр получения FIFO
	CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0); // Сбрасываем флаг прерывания
	}

	if (CAN_GetITStatus(CAN2, CAN_IT_FF0)==SET)  // Прерывание при заполнении буфера FIFO 0
	{
	CAN_ClearITPendingBit(CAN2, CAN_IT_FF0);
	UART1_PutString ("CAN2_FIFO0_full\r\n");
	CAN_ClearFlag(CAN2, CAN_FLAG_FF0); //после обработки сбросить флаг ошибки
	}

	if (CAN_GetITStatus(CAN2, CAN_IT_FOV0)==SET)  // Прерывание при переполнении буфера FIFO 0
	{
	CAN_ClearITPendingBit(CAN2, CAN_IT_FOV0);
	UART1_PutString ("CAN2_FIFO0_overflowing\r\n");
	CAN_ClearFlag(CAN2, CAN_FLAG_FOV0); //после обработки сбросить флаг ошибки
	}

}

//========================================================================================//
void CAN2_RX1_IRQHandler ()
{
	GPIO_SetBits (LED_GPIO_Port, LED_Pin);

	if (CAN_GetITStatus(CAN2, CAN_IT_FMP1) == SET)  // Прерывание получения пакета в буфер FIFO 1
	{
	CAN_Receive(CAN2, CAN_FIFO1, &Rx_buf2); //получаем сообщение
	UART1_PutString ("CAN2_get_FIFO1\r\n");

	TxMessage.DLC = Rx_buf2.DLC;
	TxMessage.ExtId = Rx_buf2.ExtId;
	TxMessage.IDE = Rx_buf2.IDE;
	TxMessage.RTR = Rx_buf2.RTR;
	TxMessage.StdId = Rx_buf2.StdId;
	TxMessage.Data [0] = Rx_buf2.Data [0];
	TxMessage.Data [1] = Rx_buf2.Data [1];
	TxMessage.Data [2] = Rx_buf2.Data [2];
	TxMessage.Data [3] = Rx_buf2.Data [3];
	TxMessage.Data [4] = Rx_buf2.Data [4];
	TxMessage.Data [5] = Rx_buf2.Data [5];
	TxMessage.Data [6] = Rx_buf2.Data [6];
	TxMessage.Data [7] = Rx_buf2.Data [7];

	status_TX1 = CAN1_TxMessage (&TxMessage); //передадим сообщение по CAN1
	flag_CAN2_Rx = 1; //установка флага получения сообщения по CAN2

	CAN_FIFORelease (CAN2, CAN_FIFO1); //очищает указанный регистр получения FIFO
	CAN_ClearITPendingBit(CAN2, CAN_IT_FMP1); // Сбрасываем флаг прерывания
	}

}

//=========================================================================================================//
void CAN2_TX_IRQHandler(void)
{

	if (CAN_GetITStatus(CAN2,CAN_IT_TME))
	{
		CAN_ClearITPendingBit(CAN2,CAN_IT_TME);
		GPIO_ResetBits (LED_GPIO_Port, LED_Pin);
		UART1_PutString ("CAN2_put_ok\r\n");
	}
}

//=================================прерывание по изменению состояния CAN2=================================//
void CAN2_SCE_IRQHandler(void)
{

	uint8_t errorcode = 0;

if (CAN_GetITStatus(CAN2,CAN_IT_ERR)==SET) //устанавливается при возникновении любой ошибки шины
	{
	CAN_ClearITPendingBit(CAN2,CAN_IT_ERR);
	UART1_PutString ("CAN2_error\r\n");
	if (CAN_GetITStatus(CAN2,CAN_IT_EWG)==SET)  //предупреждение о том, что один из счетчиков ошибок достиг 96 или более ошибок
		{
		CAN_ClearITPendingBit(CAN2,CAN_IT_EWG);
		UART1_PutString ("CAN2_warning_error\r\n");
		}
	if (CAN_GetITStatus(CAN2,CAN_IT_EPV)==SET)  //предупреждение о том, что один из счетчиков ошибок достиг более 127 ошибок
		{
		CAN_ClearITPendingBit(CAN2,CAN_IT_EPV);
		UART1_PutString ("CAN2_passive_error\r\n");
		}
	if (CAN_GetITStatus(CAN2,CAN_IT_BOF)==SET)  //Возникает при переходе шины в режим Bus-Off, когда любой из счетчиков ошибок превысил значение 255
		{
		CAN_ClearITPendingBit(CAN2,CAN_IT_BOF);
		UART1_PutString ("CAN2_Bus-off_error\r\n");
		}
	if (CAN_GetITStatus(CAN2, CAN_IT_LEC)==SET) // Прерывание при ошибке приема передачи сообщения
		{
		CAN_ClearITPendingBit(CAN2, CAN_IT_LEC);
//		errorcode = CAN_GetLastErrorCode(CAN2); // Получим код ошибки
//		sprintf (msg_UART_Tx, "CAN2_transceiver_error, code = %x\r\n", errorcode);
//		UART1_PutString (msg_UART_Tx);
		UART1_PutString ("CAN2_transceiver_error\r\n");
		CAN_ClearFlag(CAN2, CAN_FLAG_LEC); //после обработки сбросить флаг ошибки
		}
	}
else
	{
	if (CAN_GetITStatus(CAN2, CAN_IT_WKU)==SET) // Прерывание при выходе из "спящего" режима
		{
		CAN_ClearITPendingBit(CAN2, CAN_IT_WKU);
		UART1_PutString ("CAN2_Wake_up\r\n");
		CAN_ClearFlag(CAN2, CAN_FLAG_WKU); //после обработки сбросить флаг ошибки
		}
	if (CAN_GetITStatus(CAN2, CAN_IT_SLK)==SET) // Прерывание при переходе в "спящий" режим
		{
		CAN_ClearITPendingBit(CAN2, CAN_IT_SLK);
		UART1_PutString ("CAN2_Sleep\r\n");
		CAN_ClearFlag(CAN2, CAN_FLAG_SLAK); //после обработки сбросить флаг ошибки
		}
	}
}

//======================================================================================================//

int main(void)
{
// объявления для PLL
uint8_t RCC_PLLM = 4; // предделитель PLL
uint32_t  RCC_PLLN = 144 ; // умножитель PLL
uint32_t  RCC_PLLP = 2; // постделитель PLL
uint32_t RCC_PLLQ = 7;
uint8_t CPU_WS = 2; //время задержки для работы с Flash памятью
uint8_t status_PLL = PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP, RCC_PLLQ); //инициализируем генератор PLL

LED_ini (); //инициализируем LED
GPIO_SetBits (LED_GPIO_Port, LED_Pin);
UART1_ini_spl (BAUDRATE); //инициализируем UART1
if (status_PLL)
	UART1_PutString("init PLL ok\r\n");
delay_us (DELAY);
init_CAN1 (); //инициализируем CAN1
init_CAN2 (); //инициализируем CAN2
UART1_PutString ("CAN gateway start\r\n");
RCC_GetClocksFreq (&RCC_Clocks); //возвращает частоты SYSCLK, HCLK, PCLK1 и PCLK2
sprintf (msg_UART_Tx, "%lu, %lu, %lu, %lu\r\n", RCC_Clocks.SYSCLK_Frequency, RCC_Clocks.HCLK_Frequency, RCC_Clocks.PCLK1_Frequency, RCC_Clocks.PCLK2_Frequency);
UART1_PutString (msg_UART_Tx);
GPIO_ResetBits (LED_GPIO_Port, LED_Pin);


while (1)

	{
	if (flag_CAN1_Rx == 1) //если сообщение принято по шине САN1
		{
		if (!status_TX2) //проверим успешность передачи по шине САN2
			UART1_PutString ("CAN2_put_ok\r\n");
		else
			{
			sprintf (msg_UART_Tx, "CAN2_put_error, code = %x\r\n", status_TX2);
			UART1_PutString (msg_UART_Tx);
			}
		flag_CAN1_Rx = 0;
		GPIO_ResetBits (LED_GPIO_Port, LED_Pin);
		}
	if (flag_CAN2_Rx == 1)
		{
		if (!status_TX1)
			UART1_PutString ("CAN1_put_ok\r\n");
		else
			{
			sprintf (msg_UART_Tx, "CAN1_put_error, code = %x\r\n", status_TX1);
			UART1_PutString (msg_UART_Tx);
			}
		flag_CAN2_Rx = 0;
		GPIO_ResetBits (LED_GPIO_Port, LED_Pin);
		}
	}

}
