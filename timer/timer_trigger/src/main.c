/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
//--------------------------программа проверки запуска таймера по внешнему триггеру (gated mode)-----------------------------//
#include "stdio.h"
#include "stm32f4xx.h"
#include "delay_lib.h"
#include "MCU_ini.h"

#define DELAY 1000 //задержка
#define SIGNAL_A 45 //время срабатывания аларма А

#define msg_SIZE 50  //размер массива для формирования сообщений для отправки по UART
char msg [msg_SIZE]; // массив для формирования сообщений для отправки по UART
char recieve_msg [msg_SIZE]; // массив с принятыми сообщением по UART
uint8_t receive_flag = 0; //флаг принятия данных
uint8_t recieve_msg_count = 0;  //указатель на элемент массива с принятым сообщением по UART

//----------------функци¤ передачи данных по UART2--------------------//
void UART2_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // последовательно отправлем символы массива, пока не достигнут символ конца строки "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //ждём выставлени¤ флага "transmission complete" (передача завершена)
		USART_SendData(USART2, c); // передаем байт
		}
}


//функция обработки прерывания от UART2 при передачи
void TIM4_IRQHandler ()
{
	TIM_ClearITPendingBit (TIM4, TIM_IT_Update);  //очистим флаг переполнения
	UART2_PutRAMString ("test_trigger_Timer_ok\r\n");
	TIM_Cmd(TIM4, DISABLE); //отключим таймер 4
}

int main(void)
{
	// объявления для PLL (8/8х192/2=96 MHz)
	uint8_t RCC_PLLM = 8; // предделитель PLL
	uint16_t  RCC_PLLN = 192 ; // умножитель PLL
	uint32_t  RCC_PLLP = 0; // постделитель PLL = 2
	uint8_t CPU_WS = 3; //врем¤ задержки для работы с Flash памятью
	uint8_t status_PLL; //флаг готовности PLL
	status_PLL = PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP); //настройка генератора PLL (тактирование от HSE = 8MHz, SYSCLK = 96MHz, APB1=APB2=24MHz)
	SystemCoreClockUpdate ();  //обновим системную частоту (SYSCLK/8=12MHz)
	LEDS_ini (); //инициализируем LEDs
	UART2_ini (); //инициализируем UART2
	if (status_PLL) //если генератор PLL завёлся правильно
		UART2_PutRAMString ("PLL ok\r\n");
	else
		UART2_PutRAMString ("PLL error\r\n");
	UART2_PutRAMString ("test_Timer4\r\n");
	GPIO_trigger_init ();
	ini_TIMER4_trigger ();
	GPIO_WriteBit(GPIOE, GPIO_Pin_0, SET);
	delay_us(10);
	GPIO_WriteBit(GPIOE, GPIO_Pin_0, RESET);

	while (1)
	{
		delay_ms (DELAY*2);  //задержка 2 с
		GPIO_WriteBit(GPIOE, GPIO_Pin_0, SET);
		delay_us(10);
		GPIO_WriteBit(GPIOE, GPIO_Pin_0, RESET);

	}
}
