//UART. Передача и приём данных
#include "stm32f4xx.h"
#include "MCU_ini.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define DELAY 500
uint16_t delay_count = 0;

#define msg_SIZE 60
static char msg[msg_SIZE]; // массив для формирования сообщений для вывода по UART
static char recieve_msg [msg_SIZE]; // массив для полученных по UART сообщений
uint8_t msg_count = 0;  //указатель на массив полученных данных
static uint8_t recieve_msg_count = 0; //указатель на массив принятых данных
__IO uint8_t receive_flag = 0;  //флаг принятия данных
__IO uint8_t device_num = 0; //номер светодиода
char *recieve_msg_flag = recieve_msg;

//функция обработки прерывания от таймера SysTick
void SysTick_Handler ()
{
	if (delay_count > 0) //пока значение delay_count больше 0,
		{
		delay_count--; //уменьшаем значение delay_count
		}
}

//функция передачи данных по UART2
void UART2_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // отправляем символы строки последовательно, пока не достигнут символ конца строки "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //ждём выставления флага "transmission complete"
		USART_SendData(USART2, c); // передаем байт
		}
}

//функция задержки
void delay_ms (uint16_t delay_temp)
	{
	delay_count = delay_temp;
	while (delay_count) {} //пока значение delay_count не равно 0, продолжаем ждать
	}

//функция обработки прерывания от UART2 при передачи
void USART2_IRQHandler (void)
	{
	if (USART_GetITStatus(USART2, USART_IT_TXE)) //если установлен флаг USART_IT_TXE (Данные переданы в регистр сдвига)
		{
		USART_ClearITPendingBit(USART2, USART_IT_TXE); //очистим флаг
		if (msg [msg_count] != '\0') //если строка не достигла конца
			{
			USART_SendData(USART2, msg [msg_count]); //передаём бит
			msg_count++; //увеличивем указатель на 1
			}
		else
			{
			msg_count = 0; //обнуляем указатель
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE); //отключаем прерывание
			}
		}
	if (USART_GetITStatus(USART2, USART_IT_RXNE)) //если установлен флаг USART_IT_RXNE
		{
		recieve_msg [recieve_msg_count] = USART_ReceiveData(USART2);  //записываем в массив принятые данные
		recieve_msg_count++;  //увеличиваем указатель
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); //очистим флаг
		if((recieve_msg[recieve_msg_count-2] == 0x0D) && (recieve_msg[recieve_msg_count-1] == 0x0A))
			{
			receive_flag = 1; // 1- флаг принятия данных
			UART2_PutRAMString("get!\r");
			}
		}
	}

//функция подготовки массива для передачи в UART2 и включения прерывания
void ini_message_to_UART2 (char *str)
{
	memset (msg, '\0', msg_SIZE);
	for (uint8_t i = 0; i < msg_SIZE; i++)
	{
		msg [i] = *str++;
		if (*str == '\0')
			break;
	}
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

//функция обработки комманды, полученной по UART2
uint8_t recieve_comand_UART ( uint8_t flag)
{
	if (flag)
		{
		GPIO_ResetBits (GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
		device_num = atoi(recieve_msg); // преобразуем номер устройства из символов в числа
		memset (recieve_msg, '\0', msg_SIZE);
		switch(device_num) // проверим принятую команду
			{
			case 1: GPIO_SetBits (GPIOD, GPIO_Pin_12); //включаем 1
					delay_ms (DELAY);
					GPIO_ResetBits (GPIOD, GPIO_Pin_12);
					break;
			case 2: GPIO_SetBits (GPIOD, GPIO_Pin_13);
					delay_ms (DELAY);
					GPIO_ResetBits (GPIOD, GPIO_Pin_13); //включаем 2
					break;
			case 3: GPIO_SetBits (GPIOD, GPIO_Pin_14); //включаем 3
					delay_ms (DELAY);
					GPIO_ResetBits (GPIOD, GPIO_Pin_14);
					break;
			case 4: GPIO_SetBits (GPIOD, GPIO_Pin_15); //включаем 4
					delay_ms (DELAY);
					GPIO_ResetBits (GPIOD, GPIO_Pin_15);
					break;
			default: UART2_PutRAMString("error!\r");; // флаг ошибки: >0 - ошибка, 0 - нет
					break;
			}
		flag = 0; //обнуляем флаг принятия команды по UART2
		recieve_msg_count = 0; //обнуляем указатель на массив принятых данных по UART2
		}
	return flag;
}

int main(void)
{
	// объявления для PLL
	uint8_t RCC_PLLM = 16; // предделитель PLL
	uint32_t  RCC_PLLN = 336 ; // умножитель PLL
	uint32_t  RCC_PLLP = 0; // постделитель PLL
	uint8_t CPU_WS = 5; //время задержки для работы с Flash памятью
	PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP); //инициализируем генератор PLL
	SystemCoreClockUpdate ();  //обновим системную частоту
	LEDS_ini (); //инициализируем LEDs
	UART2_ini (); //инициализируем UART2
	SysTick_Config (SystemCoreClock/1000);  //прерывания раз в 1 мс
	sprintf(msg,"\r\nUART2 TX with IRQ programm start\r\n");
//	USART_ClearITPendingBit(USART2, USART_IT_RXNE); //очистим флаг
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);  //включим прерывание  на передачу при пустом регистре данных

	while (1)
	{
	GPIO_SetBits (GPIOD, GPIO_Pin_12);
	GPIO_ResetBits (GPIOD, GPIO_Pin_15);
	ini_message_to_UART2 ("leds_1\r\n");
	receive_flag = recieve_comand_UART (receive_flag);
	delay_ms (DELAY);
	GPIO_ResetBits (GPIOD, GPIO_Pin_12);
	GPIO_SetBits (GPIOD, GPIO_Pin_13);
	ini_message_to_UART2 ("leds_2\r\n");
	receive_flag = recieve_comand_UART (receive_flag);
	delay_ms (DELAY);
	GPIO_ResetBits (GPIOD, GPIO_Pin_13);
	GPIO_SetBits (GPIOD, GPIO_Pin_14);
	ini_message_to_UART2 ("leds_3\r\n");
	receive_flag = recieve_comand_UART (receive_flag);
	delay_ms (DELAY);
	GPIO_SetBits (GPIOD, GPIO_Pin_15);
	GPIO_ResetBits (GPIOD, GPIO_Pin_14);
	ini_message_to_UART2 ("leds_4\r\n");
	receive_flag = recieve_comand_UART (receive_flag);
	delay_ms (DELAY);
	}
}
