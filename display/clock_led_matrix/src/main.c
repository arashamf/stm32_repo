/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
//----------------------------программа вывода данных по SPI на светодиодную матрицу------------------------------------//
#include "stm32f4xx.h"
#include "SPI.h"
#include "MCU_ini.h"
#include "delay_lib.h" //библиотека функций с задержками
#include "string.h" //библиотека с функциями обработки строк
#include "stdio.h"
#include "MAX7219.h"
#include "led_matrix.h" //массивы с цифрами
#include "RTC_ini.h"

#define DELAY 500

extern RTC_TimeTypeDef RTC_set_time; //шаблон структуры установки времени

//extern enum displays {display1=1, display2, display3, display4};

#define msg_SIZE 50  //размер массива для формирования сообщений для отправки по UART
char msg [msg_SIZE]; // массив для формирования сообщений для отправки по UART
char recieve_msg [msg_SIZE]; // массив с принятыми данными по UART
uint8_t receive_flag = 0; //флаг принятия данных по UART
uint8_t recieve_msg_count = 0; //указатель на массив с принятыми данными по UART

void UART2_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // последовательно отправлем символы массива, пока не достигнут символ конца строки "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //ждём выставлени¤ флага "transmission complete" (передача завершена)
		USART_SendData(USART2, c); // передаем байт
		}
}

//функция обработки прерывания от UART2 при приёме
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
	while (1) //цикл опроса пользователя, до тех пока пока не приняты корректные данные
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
			if (UART_RTC_set_time (recieve_msg)) //если функция установки времени не вернула ошибку
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

int main(void)
{
	//объявим переменные передачи времени на дисплеи матрицы
	uint8_t high_digit_hour, low_digit_hour, high_digit_minute, low_digit_minute, high_digit_second, low_digit_second;
	// объявления для PLL (8/8х192/8=24 MHz)
	uint8_t RCC_PLLM = 8; // предделитель PLL
	uint16_t  RCC_PLLN = 192 ; // умножитель PLL
	uint32_t  RCC_PLLP = 3; // постделитель PLL = 8
	uint8_t CPU_WS = 0; //время задержки для работы с Flash памятью (1 такт CPU) (RM0090 стр. 81)
	uint8_t status_PLL; //флаг готовности PLL
	status_PLL = PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP); //настройка генератора PLL (F_HSE = 8MHz, F_SYSCLK = 24MHz, F_APB1=F_APB2=6MHz)
	SystemCoreClockUpdate ();  //обновим системную частоту (SYSCLK/8=12MHz)
	LEDS_ini (); //инициализируем LEDs
	UART2_ini (); //инициализируем UART2
	if (status_PLL) //если генератор PLL завёлся правильно
		UART2_PutRAMString ("PLL ok\r\n");
	else
		UART2_PutRAMString ("PLL error\r\n");
	SPI2_ini ();
	UART2_PutRAMString ("RTC start\r\n");
	UART2_PutRAMString ("please, set time\r\n");
	RTC_clock_ini_SPL (); //инициализация RTC
	check_and_set_time (); //висим в этой функции, пока не будут получены корректные данные времени
	init_MAX7219 ();

	while (1)
	{
		/*if (receive_flag)
			{
			if (!strcmp (recieve_msg, "test\r\n"))
				{
				UART2_PutRAMString ("test displays start\r\n");
				test_displays (DELAY);
				}
			receive_flag = 0;
			}*/
		RTC_GetTime (RTC_Format_BIN, &RTC_set_time); //заполним структуры текущими данными времени (ч, м, с)
		sprintf(msg,"time: %2d:%2d:%2d\r\n", RTC_set_time.RTC_Hours, RTC_set_time.RTC_Minutes, RTC_set_time.RTC_Seconds);
		UART2_PutRAMString (msg); // выводим строку по UART

		high_digit_hour = RTC_set_time.RTC_Hours / 10; //вычислим старший разряд часов
		low_digit_hour = RTC_set_time.RTC_Hours % 10; //вычислим младший разряд часов
		high_digit_minute = RTC_set_time.RTC_Minutes /10; //вычислим старший разряд минут
		low_digit_minute = RTC_set_time.RTC_Minutes % 10; //вычислим младший разряд минут
		high_digit_second = RTC_set_time.RTC_Seconds / 10; //вычислим старший разряд секунд
		low_digit_second = RTC_set_time.RTC_Seconds % 10; //вычислим младший разряд секунд

		putchar_display (display1, reg_string, led_digit, low_digit_minute); //младший разряд минуты, соответсвует номеру цифры в массиве
		putchar_display (display2, reg_string, led_digit, high_digit_minute);
		putchar_display (display3, reg_string, led_digit, low_digit_hour);
		putchar_display (display4, reg_string, led_digit, high_digit_hour);

		delay_ms (1000);
	}
}
