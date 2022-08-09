//----------------------------программа вывода данных по SPI на светодиодную матрицу------------------------------------//
#include "stm32f4xx.h"
#include "SPI.h"
#include "MCU_ini.h"
#include "delay_lib.h" //библиотека функций с задержками
#include "string.h" //библиотека с функциями обработки строк
#include "MAX7219.h"
#include "led_matrix.h" //массивы с цифрами

#define DELAY 500

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

int main(void)
{
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
	UART2_PutRAMString ("matix module programm start\r\n");
	init_MAX7219 ();
//	shutdown_display (display2); //отключим дисплей 2
//	shutdown_display (display3); //отключим дисплей 3
	shutdown_display (display4); //отключим дисплей 4



	while (1)
	{
		if (receive_flag)
			{
			if (!strcmp (recieve_msg, "test\r\n"))
				{
				UART2_PutRAMString ("test displays start\r\n");
				test_displays (DELAY);
				}
			receive_flag = 0;
			}

		putchar_display (display1, reg_string, numb_1);
		putchar_display (display2, reg_string, numb_2);
		putchar_display (display3, reg_string, numb_3);
		delay_ms (800);
		putchar_display (display1, reg_string, numb_4);
		putchar_display (display2, reg_string, numb_5);
		putchar_display (display3, reg_string, numb_6);
		delay_ms (800);
		putchar_display (display1, reg_string, numb_7);
		putchar_display (display2, reg_string, numb_8);
		putchar_display (display3, reg_string, numb_9);
		delay_ms (800);
	}
}
