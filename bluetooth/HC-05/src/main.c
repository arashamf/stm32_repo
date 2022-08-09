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
#include "string.h"
#include "MCU_ini.h"
#include "delay_lib.h"
#include "stm32f4xx_rcc.h"

#define DELAY 500
#define msg_SIZE 50  //������ ������� ��� ������������ ��������� ��� �������� �� UART
char msg [msg_SIZE]; // ������ ��� ������������ ��������� ��� �������� �� UART
char recieve_msg [msg_SIZE]; // ������ � ��������� ������� �� UART
uint8_t receive_flag = 0; //���� �������� ������ �� UART
uint8_t recieve_msg_count = 0; //��������� �� ������ � ��������� ������� �� UART
//uint16_t delay_count = 0;

//--------------------------------------�-� �������� ������ �� UART3----------------------------------------//
void UART3_putstring(char *str)
{
	char c;
	while((c = *str++)) // ��������������� ��������� ������� �������, ���� �� ��������� ������ ����� ������ "\0"
		{
		while (!USART_GetFlagStatus(USART3, USART_FLAG_TC)) {}; //��� ���������� ����� "transmission complete" (�������� ���������)
		USART_SendData(USART3, c); // �������� ����
		}
}

//------------------------------�-� �������� ������ �� UART2 �� ������ HC-02--------------------------------//
void UART2_putstring(char *str)
{
	char c;
	while((c = *str++)) // ��������������� ��������� ������� �������, ���� �� ��������� ������ ����� ������ "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //��� ���������� ����� "transmission complete" (�������� ���������)
		USART_SendData(USART2, c); // �������� ����
		}
}

//----------------------------------������� ��������� ���������� �� UART3 ��� �����----------------------------------//
void USART2_IRQHandler ()
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE)) //���� ���������� ���� USART_IT_RXNE
	{
		recieve_msg [recieve_msg_count] = USART_ReceiveData(USART2);  //���������� � ������ �������� ������
		recieve_msg_count++;  //����������� ���������
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); //������� ����
		if((recieve_msg[recieve_msg_count-2] == 0x0D) && (recieve_msg[recieve_msg_count-1] == 0x0A)) //�������� ������� �������� '\r' � '\n'
		{
			receive_flag = 1; // 1- ��������� ������� ���������
			recieve_msg_count = 0; //������� ��������� �� ������� ������� � �������� ����������
		}
	}
}

//---------------------------�-�� �������� ���������� �������-------------------------------------//
uint8_t check_command ()
{
	uint8_t flag_error = 1; //���� ������. 0: ������ ������, 1: ������ ���������
	if (!(strncmp (recieve_msg, "led1on", 6))) // �������� 1 �������, strncmp ���������� ������ n �������� �����. ���������� "0", ���� ������ ���������, "< 0" - ���� ������1 ������ �����2, "> 0" - ���� ������1 ������ ������ 2*/
		{
		GPIO_SetBits(GPIOD, GPIO_Pin_12);
		}
	else
		{
		if (!(strncmp (recieve_msg, "led1off", 7)))
			{
			GPIO_ResetBits(GPIOD, GPIO_Pin_12);
			}
		else
			flag_error = 0;
		}
return flag_error;
}

//-----------------------------�-� ��������� ����������� ���������-----------------------------------------------//
void HC02_message ()
{
	if (receive_flag)
	{
		if (check_command ())
			{
			UART3_putstring (recieve_msg);
			}
		else
			{
			UART3_putstring ("command_error\r\n");
			}
		memset(recieve_msg, 0, strlen (recieve_msg));
		receive_flag = 0;
	}
}


int main(void)
{
	// ���������� ��� PLL
	uint8_t RCC_PLLM = 8; // ������������ PLL
	uint32_t  RCC_PLLN = 256 ; // ���������� PLL
	uint32_t  RCC_PLLP = 0; // ������������ PLL (� ������ ������ ����� 2)
	uint8_t CPU_WS = 3; //����� �������� ��� ������ � Flash �������
	uint8_t status_PLL = PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP); //�������������� ��������� PLL
	SystemCoreClockUpdate ();  //������� ��������� �������
	LEDS_ini (); //�������������� LEDs
	UART2_ini ();
	UART3_ini (); //�������������� UART3
	if (status_PLL) //���� ��������� PLL ������ ���������
			UART3_putstring ("PLL ok\r\n");
		else
			UART3_putstring ("PLL error\r\n");
	UART3_putstring ("HC-05 programm start\r\n");


	for(;;)
	{

		HC02_message ();
		GPIO_ToggleBits (GPIOD, GPIO_Pin_15);
		UART2_putstring ("hello\r\n");
		delay_ms (DELAY);


	}
}
