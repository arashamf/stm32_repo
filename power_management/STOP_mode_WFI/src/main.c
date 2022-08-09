/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
//-------------------------программа перехода в режим останова и выхода из него по прерыванию от кнопки-------------------------------------//
#include "stm32f4xx.h"
#include "delay_lib.h"
#include "MCU_ini.h"
#include "RTC_ini.h"

#define DELAY 250
#define msg_SIZE 50  //размер массива для формирования сообщений для отправки по UART
char msg [msg_SIZE]; // массив для формирования сообщений для отправки по UART

// переменные для обработки дребезга контактов кнопки
#define KEY_Bounce_Time 400 // время дребезга + автоповтора в мс
__IO uint16_t KEY_Bounce_Time_Cnt; // счетчик времени дребезга
__IO uint8_t KEY_Press_Flag = 0; // флаг нажатия на кнопку: 0 - не нажата; 1 - нажата

void UART2_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // последовательно отправлем символы массива, пока не достигнут символ конца строки "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //ждём выставлени¤ флага "transmission complete" (передача завершена)
		USART_SendData(USART2, c); // передаем байт
		}
}

// обработка прерывания EXTI0 от кнопки
void EXTI0_IRQHandler(void)
{
SET_BIT (EXTI->PR, EXTI_PR_PR0); // сброс флага прерывания
UART2_PutRAMString ("i don`t sleep)\r\n");
}

int main(void)
{
	uint8_t  status_RCC = 0;
	status_RCC = HSE_ini (); //тактирование от HSE = 8MHz
	LEDS_ini (); //инициализируем LEDs
	UART2_ini (); //инициализируем UART2
	if (status_RCC) //если генератор HSE завёлся правильно
		UART2_PutRAMString ("HSE ok\r\n");
	else
		UART2_PutRAMString ("HSE error\r\n");
	UART2_PutRAMString ("wake_up_button_programm start\r\n");
	ini_button_B1 (); //настроим кнопку с прерыванием
	init_stop_mode (); //настроим режим останова
	__WFI(); // переходим в режим останова c выходом по прерыванию

	while(1)
	{
		//помигаем при пробуждении
		for (uint8_t i = 0; i < 4; i++)
			{
			GPIOD->BSRRL |= GPIO_BSRR_BS_12; //включаем 12 пин
			delay_ms (DELAY);
			GPIOD->BSRRH |= GPIO_BSRR_BS_12; //выключаем 12 пин
			delay_ms (DELAY);
			}
		UART2_PutRAMString ("start mode STOP\r\n");
		__WFI(); // переходим в режим останова
	}
}
