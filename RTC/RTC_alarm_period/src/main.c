/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
//-------------------------------инициализация RTC c прерываниями от 1 будильника с периодическими операциями------------------------------//
#include "stdio.h"
#include "stm32f4xx.h"
#include "delay_lib.h"
#include "MCU_ini.h"
#include "RTC_ini.h"

#define DELAY 1000 //задержка
#define SIGNAL_A 45 //время срабатывания аларма А

#define msg_SIZE 50  //размер массива для формирования сообщений для отправки по UART
char msg [msg_SIZE]; // массив для формирования сообщений для отправки по UART
char recieve_msg [msg_SIZE]; // массив с принятыми сообщением по UART
uint8_t receive_flag = 0; //флаг принятия данных
uint8_t recieve_msg_count = 0;  //указатель на элемент массива с принятым сообщением по UART

uint8_t flag_ALM_A = 0; //флаг срабатывания прерывания от аларма А

extern RTC_TimeTypeDef RTC_set_time; //инициализируем шаблон структуры установки времени

//----------------массив для формирования синусоиды------------------//
uint16_t sinus[72]={2225,2402,2577,2747,2912,3070,3221,3363,3494,3615,
					3724,3820,3902,3971,4024,4063,4085,4095,4085,4063,
					4024,3971,3902,3820,3724,3615,3495,3363,3221,3071,
					2912,2747,2577,2403,2226,2047,1869,1692,1517,1347,
					1182,1024, 873, 731, 600, 479, 370, 274, 192, 124,
					70,	 31,	10,	0,	 10,  31,  70,  123, 192, 274,
					370, 479, 599, 731, 873,1023,1182,1347,1517, 1691,
					1868,2047
					};
uint8_t count = 0;
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

//---------------------------ф-ия получения и проверки настроек времени RTC-------------------------------------//
void check_and_set_time ()
{
	uint8_t count = 0; //значение счётчика
	while (!receive_flag) //цикл опроса пользователя, до тех пока пока не приняты корректные данные
	{
		if (((count++) % 4) == 0) //выводим сообщения при count = 4, т.е. раз в 2 с
			{
			UART2_PutRAMString ("please, set time\r\n");
			UART2_PutRAMString ("example: 121543\r\n");
			}
		GPIO_WriteBit(GPIOD, (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15), Bit_SET); //помигаем светодиодами
		delay_ms (DELAY/4); // задержка 1/4 c
		GPIO_WriteBit(GPIOD, (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15), Bit_RESET);
		delay_ms (DELAY/4); // задержка 1/4 c
		if (receive_flag) //если флаг принятия данных по UART установлен
			{
			if (UART_RTC_set_time_SPL (recieve_msg)) //если функция установки не вернула ошибку
				{
				receive_flag = 0; //обнулим флаг принятия данных
				break;			 //выйдем из цикла опроса
				}
			else    			//если функция установки времени вернуло 0
				{
				UART2_PutRAMString ("data time error!\r\n"); //предупредим об ошибке и продолжим цикл опроса
				}
			receive_flag = 0; //обнулим флаг принятия данных
			}
		if (count == 4) //если счётчик равен 4
			count = 0; //сбросим значение счётчика
	}
}

//---------------------ф-ия обработки прерывание от Аларма А--------------------------------------------//
void RTC_Alarm_IRQHandler(void)
{
	if (READ_BIT (RTC->ISR, RTC_ISR_ALRAF))
		{
		CLEAR_BIT (RTC->ISR, RTC_ISR_ALRAF); // сбросим флаг будильника A (сбрасывается путем записи "0")
		SET_BIT (EXTI->PR, EXTI_PR_PR17); // сбросим флаг линии внешнего прерывания EXTI17 (сбрасывается путем записи "1", а не "0")
		flag_ALM_A = 1; // установить флаг срабатывания прерывания будильника А
		}
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
	RTC_clock_ini_SPL (); //инициализация RTC
	UART2_PutRAMString ("RTC programm start\r\n");
	check_and_set_time (); //висим в этой функции, пока не будут получены корректные данные
	NVIC_DisableIRQ(USART2_IRQn); //отключим прерывание от USART2
	ALARMA_ini_IRQ (SIGNAL_A); //установим будильник A на каждую 45 секунду
	RTC_set_date_SPL (); //установим дату
	DAC_ini ();  //запустим ЦАП 1 с синусоидой на выходе

	while (1)
	{
		if(flag_ALM_A)
			{
			UART2_PutRAMString("ALARM A!!!\r\n"); // выводим строку по UART
			ini_TIMER4 (5000); //длительность сигнала 10с
			while (!TIM_GetFlagStatus(TIM4, TIM_FLAG_Update))
				{
				WRITE_REG (DAC->DHR12R1, sinus [count]);
				SET_BIT (DAC->SWTRIGR, DAC_SWTRIGR_SWTRIG1); // разрешим программный запуск ЦАП1
				while (READ_BIT (DAC->SWTRIGR, DAC_SWTRIGR_SWTRIG1)) {}; // ожидаем окончания преобразования ЦАП1
				if ((count++) > 72)
					count = 0;
				}
			flag_ALM_A = 0; //обнулим флаг
			count = 0;
			TIM_Cmd(TIM4, DISABLE); //включим таймер 4
			}
		RTC_GetTime (RTC_Format_BIN, &RTC_set_time); //заполним структуры текущими данными времени (ч, м, с)
		sprintf(msg,"time: %2d:%2d:%2d\r\n", RTC_set_time.RTC_Hours, RTC_set_time.RTC_Minutes, RTC_set_time.RTC_Seconds);
		UART2_PutRAMString (msg); // выводим строку по UART
		delay_ms (1000); // задержка 1 c
	}
}
