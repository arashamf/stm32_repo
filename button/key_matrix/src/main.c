/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
//-------------------------------программа обработки матричной клавиатуры---------------------------------//
#include "stdio.h"
#include "stm32f4xx.h"
#include "delay_lib.h"
#include "MCU_ini.h"
#include "main.h"

#define DELAY 1000 //задержка

#define msg_SIZE 50  //размер массива для формирования сообщений для отправки по UART
char msg [msg_SIZE]; // массив для вывода сообщения по UART

// состояние кнопки
#define key_state_off 0 // не нажата
#define key_state_bounce 1 // дребезг
#define key_state_on 2 // нажата
#define key_state_autorepeat 3 //режим автоповтора
__IO uint8_t key_state = key_state_off; // начальное состояние кнопки - не нажата

// счетчик времени дребезга
#define key_bounce_time_cnt 50 // время дребезга в мс
#define key_autorepeat_time_cnt 300 // время автоповтора в мс
__IO uint16_t key_repeat_time_cnt; // счетчик времени повтора

__IO uint16_t kscan_code = 0; //код нажатой кнопки
__IO uint8_t kscan_step; // шаг (фаза) сканирования
//__IO uint16_t kscan_code_copy;

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

//-----------------------обработка прерываний от таймера 4--------------------------------//
void TIM4_IRQHandler()
{
	CLEAR_BIT (TIM4->SR, TIM_SR_UIF); // сбросим флаг прерывания
	if(key_state == key_state_off) //если кнопка была отпущена - Ожидаем нажатие
	{
	// получаем код нажатой кнопки
		if(!READ_BIT (KDATA_port, KDATA0_bit))
		{
			key_state =  key_state_on;
			switch (kscan_step)
				{
				case 0:
					kscan_code = KEY_s0_d0;
					break;
				case 1:
					kscan_code = KEY_s1_d0;
					break;
				case 2:
					kscan_code = KEY_s2_d0;
					break;
				case 3:
					kscan_code = KEY_s3_d0;
					break;
				default:
					break;
				}
		}
		else
			if (!READ_BIT(KDATA_port, KDATA1_bit))
			{
				key_state =  key_state_on;
				switch (kscan_step)
					{
					case 0:
						kscan_code = KEY_s0_d1;
						break;
					case 1:
						kscan_code = KEY_s1_d1;
						break;
					case 2:
						kscan_code = KEY_s2_d1;
						break;
					case 3:
						kscan_code = KEY_s3_d1;
						break;
					default:
						break;
					}
			}
			else
				if (!READ_BIT(KDATA_port, KDATA2_bit))
				{
					key_state =  key_state_on;
					switch (kscan_step)
						{
						case 0:
							kscan_code = KEY_s0_d2;
							break;
						case 1:
							kscan_code = KEY_s1_d2;
							break;
						case 2:
							kscan_code = KEY_s2_d2;
							break;
						case 3:
							kscan_code = KEY_s3_d2;
							break;
						default:
							break;
						}
				}
	}
	if (key_state ==  key_state_on)
	{
		key_state = key_state_bounce; // состояние кнопки - дребезг
		key_repeat_time_cnt = key_bounce_time_cnt; // счетчик времени дребезга - устанавливаем
	}

	if(key_state == key_state_bounce) // пропускаем интервал дребезга
	{
		if(key_repeat_time_cnt)
			key_repeat_time_cnt--; // счетчик времени дребезга - уменьшаем
		else // дребезг кончился
		{
			key_state = key_state_autorepeat; // кнопка нажата
			key_repeat_time_cnt = key_autorepeat_time_cnt; // счетчик времени автоповтора - устанавливаем
		}
	}
	if (key_state == key_state_autorepeat)
	{
		if(key_repeat_time_cnt)
		{
			key_repeat_time_cnt--; // уменьшаем счетчик автоповтора
		}
		else
			if((READ_BIT(KDATA_port, KDATA0_bit)) && (READ_BIT(KDATA_port, KDATA1_bit)) && (READ_BIT(KDATA_port, KDATA2_bit))) // если кнопка отпущена
			{
				key_state = key_state_off; // кнопка отпущена
			}
			else // кнопка продолжает удерживаться
			{
				key_repeat_time_cnt = key_autorepeat_time_cnt; // установим счетчик автоповтора
			}
	}
	switch(kscan_step) // сканирование клавиатуры
		{
		case 0:
			kscan_step = 1; // шаг (фаза) сканирования
			SET_BIT(KSCAN_port, KSCAN0_bit); // линия не активирована
			CLEAR_BIT(KSCAN_port, KSCAN1_bit); // линия активирована
			break;
		case 1:
			kscan_step = 2; // шаг (фаза) сканирования
			SET_BIT(KSCAN_port, KSCAN1_bit); // линия не активирована
			CLEAR_BIT(KSCAN_port, KSCAN2_bit); // линия активирована
			break;
		case 2:
			kscan_step = 3; // шаг (фаза) сканирования
			SET_BIT(KSCAN_port, KSCAN2_bit); // линия не активирована
			CLEAR_BIT(KSCAN_port, KSCAN3_bit); // линия активирована
			break;
		case 3:
			kscan_step = 0; // шаг (фаза) сканирования
			CLEAR_BIT(KSCAN_port, KSCAN0_bit); // линия активирована
			SET_BIT(KSCAN_port, KSCAN3_bit); // линия не активирована
			break;
		default:
			break;
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

	UART2_PutRAMString ("Key matrix programm start\r\n");

	// начальная установка сканирования
	kscan_step = 0; // шаг (фаза) сканирования
	SET_BIT(KSCAN_port, KSCAN3_bit); // линия активирована
	SET_BIT(KSCAN_port, KSCAN3_bit); // линия не активирована
	SET_BIT(KSCAN_port, KSCAN3_bit); // линия не активирована
	SET_BIT(KSCAN_port, KSCAN3_bit); // линия не активирована

	ini_TIMER4 (); //запустим таймер 4 для опроса кнопок

	while (1)
	{
		if(kscan_code) // если кнопка нажата
		{
			sprintf(msg, "kscan_code - %x\r\n", kscan_code);
			UART2_PutRAMString (msg); // выводим код кнопки
			switch(kscan_code) // сканирование клавиатуры
					{
					case 0x7A:
						GPIO_WriteBit(GPIOD, GPIO_Pin_12, SET);
						GPIO_WriteBit(GPIOD, (GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15), RESET);
						break;
					case 0xBA:
						GPIO_WriteBit(GPIOD, GPIO_Pin_13, SET);
						GPIO_WriteBit(GPIOD, (GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15), RESET);
						break;
					case 0xDA:
						GPIO_WriteBit(GPIOD, GPIO_Pin_14, SET);
						GPIO_WriteBit(GPIOD, (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15), RESET);
						break;
					case 0xEA:
						GPIO_WriteBit(GPIOD, GPIO_Pin_15, SET);
						GPIO_WriteBit(GPIOD, (GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_12), RESET);
						break;
					default:
						GPIO_WriteBit(GPIOD, (GPIO_Pin_12 |GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15), RESET);
						break;
					}
			kscan_code = 0; // выполнили действие - сбросили флаг
		}
	}
}

