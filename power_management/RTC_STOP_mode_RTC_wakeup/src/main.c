
//-----------------программа перехода в режим останова и выхода из него по прерыванию от кнопки и от таймера wakeup---------------------//
#include "stm32f4xx.h"
#include "stdio.h"
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

// обработка прерывания EXTI0
void EXTI0_IRQHandler(void)
{
//KEY_Bounce_Time_Cnt = KEY_Bounce_Time; // счетчик времени дребезга - запустим
SET_BIT (EXTI->PR, EXTI_PR_PR0); // сброс флага прерывания
UART2_PutRAMString ("i don`t sleep\r\n");
}

//обработка прерывания от таймера Wakeup
void RTC_WKUP_IRQHandler(void)
{
CLEAR_BIT (RTC->ISR, RTC_ISR_WUTF); // сбросим флаг таймера Wakeup (сбрасывается записью "0")
SET_BIT (EXTI->PR, EXTI_PR_PR22); // сбросим флаг внешнего прерывания (сбрасывается записью "1")
}

int main(void)
{

	uint8_t  status_RCC = 0;
	status_RCC = HSI_ini (); //настройка тактирования
	SystemCoreClockUpdate ();  //обновим системную частоту
	LEDS_ini (); //инициализируем LEDs
	UART2_ini (); //инициализируем UART2
	if (status_RCC) //если генератор HSI завёлся правильно
		UART2_PutRAMString ("HSI clock ok\r\n");
	else
		UART2_PutRAMString ("HSI error\r\n");
	UART2_PutRAMString ("wake_up_button_programm start\r\n");
	ini_button_B1 ();
	RTC_ini (); //перед инициализацией wakeup таймера необходимо инициализировать RTC
	WAKEUP_ini_SPL (); //инициализация таймера wakeup
	init_stop_mode ();
	__WFI(); // переходим в спящий режим

	while(1)
	{
		//помигаем при пробуждении
		for (uint8_t i = 0; i < 6; i++)
		{
			GPIO_ToggleBits(GPIOD, GPIO_Pin_15); // включаем/отключаем светодиод
			delay_ms (DELAY/2); // задержка
		}
	UART2_PutRAMString ("start mode STOP\r\n");
	__WFI(); // переходим в спящий режим
	}
}
