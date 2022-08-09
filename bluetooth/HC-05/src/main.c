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
#define msg_SIZE 50  //размер массива для формирования сообщений для отправки по UART
char msg [msg_SIZE]; // массив для формирования сообщений для отправки по UART
char recieve_msg [msg_SIZE]; // массив с принятыми данными по UART
uint8_t receive_flag = 0; //флаг принятия данных по UART
uint8_t recieve_msg_count = 0; //указатель на массив с принятыми данными по UART
//uint16_t delay_count = 0;

//--------------------------------------ф-я отправки данных по UART3----------------------------------------//
void UART3_putstring(char *str)
{
	char c;
	while((c = *str++)) // последовательно отправлем символы массива, пока не достигнут символ конца строки "\0"
		{
		while (!USART_GetFlagStatus(USART3, USART_FLAG_TC)) {}; //ждём выставлени¤ флага "transmission complete" (передача завершена)
		USART_SendData(USART3, c); // передаем байт
		}
}

//------------------------------ф-я отправки данных по UART2 на модуль HC-02--------------------------------//
void UART2_putstring(char *str)
{
	char c;
	while((c = *str++)) // последовательно отправлем символы массива, пока не достигнут символ конца строки "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //ждём выставлени¤ флага "transmission complete" (передача завершена)
		USART_SendData(USART2, c); // передаем байт
		}
}

//----------------------------------функция обработки прерывания от UART3 при приёме----------------------------------//
void USART2_IRQHandler ()
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE)) //если установлен флаг USART_IT_RXNE
	{
		recieve_msg [recieve_msg_count] = USART_ReceiveData(USART2);  //записываем в массив принятые данные
		recieve_msg_count++;  //увеличиваем указатель
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); //очистим флаг
		if((recieve_msg[recieve_msg_count-2] == 0x0D) && (recieve_msg[recieve_msg_count-1] == 0x0A)) //проверка наличия символов '\r' и '\n'
		{
			receive_flag = 1; // 1- сообщение принято полностью
			recieve_msg_count = 0; //обнулим указатель на элемент массива с принятым сообщением
		}
	}
}

//---------------------------ф-ия проверки полученной команды-------------------------------------//
uint8_t check_command ()
{
	uint8_t flag_error = 1; //флаг ошибки. 0: ошибка данных, 1: данные корректны
	if (!(strncmp (recieve_msg, "led1on", 6))) // проверим 1 лексему, strncmp сравнивает первые n символов строк. Возвращает "0", если строки одинаковы, "< 0" - если строка1 меньше сроки2, "> 0" - если строка1 больше строки 2*/
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

//-----------------------------ф-я обработки полученного сообщения-----------------------------------------------//
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
	// объявления для PLL
	uint8_t RCC_PLLM = 8; // предделитель PLL
	uint32_t  RCC_PLLN = 256 ; // умножитель PLL
	uint32_t  RCC_PLLP = 0; // постделитель PLL (в данном случае равен 2)
	uint8_t CPU_WS = 3; //время задержки для работы с Flash памятью
	uint8_t status_PLL = PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP); //инициализируем генератор PLL
	SystemCoreClockUpdate ();  //обновим системную частоту
	LEDS_ini (); //инициализируем LEDs
	UART2_ini ();
	UART3_ini (); //инициализируем UART3
	if (status_PLL) //если генератор PLL завёлся правильно
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
