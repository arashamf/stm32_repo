
#include "stm32f4xx.h"
#include "stdio.h"
#include "string.h"
#include "../../../UART_DMA_double_buffer/CMSIS/device/MCU_ini.h"

typedef unsigned char byte;
#define DELAY 500
uint16_t delay_count = 0;
extern char message [15];

void DMA1_Stream6_IRQHandler ()
{
	if (DMA_GetITStatus (DMA1_Stream6, DMA_IT_TCIF6))
	{
		DMA_ClearITPendingBit (DMA1_Stream6, DMA_IT_TCIF6);
	}
}

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

int main(void)
{
	LEDS_ini ();
	UART2_ini (); //инициализируем UART2
//	USART2->BRR = (SystemCoreClock + (BAUDRATE/2))/BAUDRATE;  //пропускная способность 57600
	DMA1_ini ();
	SysTick_Config (SystemCoreClock/1000);  //прерывания раз в 1 мс

	while (1)
	{
	GPIO_SetBits (GPIOD, GPIO_Pin_12);
	GPIO_ResetBits (GPIOD, GPIO_Pin_15);
	sprintf (message, "green led\r\n");
	DMA_Cmd(DMA1_Stream6, ENABLE);
	delay_ms (DELAY);
	memset(message, '\0', sizeof(message));
	GPIO_ResetBits (GPIOD, GPIO_Pin_12);
	GPIO_SetBits (GPIOD, GPIO_Pin_13);
	sprintf (message, "orange led\r\n");
	DMA_Cmd(DMA1_Stream6, ENABLE);
	delay_ms (DELAY);
	memset(message, '\0', sizeof(message));
	GPIO_ResetBits (GPIOD, GPIO_Pin_13);
	GPIO_SetBits (GPIOD, GPIO_Pin_14);
	sprintf (message, "red led\r\n");
	DMA_Cmd(DMA1_Stream6, ENABLE);
	delay_ms (DELAY);
	memset(message, '\0', sizeof(message));
	GPIO_SetBits (GPIOD, GPIO_Pin_15);
	GPIO_ResetBits (GPIOD, GPIO_Pin_14);
	sprintf (message, "blue led\r\n");
	DMA_Cmd(DMA1_Stream6, ENABLE);
	delay_ms (DELAY);
	memset(message, '\0', sizeof(message));
	}
}
