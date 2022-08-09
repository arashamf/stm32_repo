//UART. Передача данных с помощью DMA. Обработка двойного буфера
#include "stm32f4xx.h"
#include "MCU_ini.h"
#include "stdio.h"
#include "string.h"

#define DELAY 500
uint16_t delay_count = 0;

char msg[60]; // массив для формирования сообщений для вывода по UART

// массивы для передачи в DMA
extern char message1 [20];
extern char message2 [20];

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
	// объявления для PLL
	uint8_t RCC_PLLM = 16; // предделитель PLL
	uint32_t  RCC_PLLN = 336 ; // умножитель PLL
	uint32_t  RCC_PLLP = 0; // постделитель PLL
	uint8_t CPU_WS = 5; //время задержки для работы с Flash памятью
	PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP); //инициализируем генератор PLL
	LEDS_ini (); //инициализируем LEDs
	UART2_ini (); //инициализируем UART2
	USART2->BRR = (APB1_CLOCK + (BAUDRATE/2))/BAUDRATE;  //пропускная способность 57600
	DMA1_ini (); //инициализируем DMA1
	SysTick_Config (SystemCoreClock/1000);  //прерывания раз в 1 мс
	sprintf(msg,"UART3 TX DMA Double buffer mode test programm start\r\n");
	UART2_PutRAMString(msg);
	for(uint8_t i = 0; i < 20; i++) //заполним массивы символами
		{
		message1 [i] = '1';
		message2 [i] = '3';
		}
//	DMA_ClearITPendingBit (DMA1_Stream6, DMA_IT_TCIF6);  //сбрасываем бит прерывания TCIF6 DMA1 Stream6
	DMA_Cmd(DMA1_Stream6, ENABLE); //включим поток 6 DMA1
	delay_ms (20);
	DMA_Cmd(DMA1_Stream6, DISABLE); //отключим поток 6 DMA1
	sprintf(msg,"UART3 TX DMA Soft Double buffer mode test programm end\r\n");
	UART2_PutRAMString (msg);

	while (1)
	{
	GPIO_SetBits (GPIOD, GPIO_Pin_12);
	GPIO_ResetBits (GPIOD, GPIO_Pin_15);
	UART2_PutRAMString("leds_1\r\n");
	delay_ms (DELAY);
	GPIO_ResetBits (GPIOD, GPIO_Pin_12);
	GPIO_SetBits (GPIOD, GPIO_Pin_13);
	UART2_PutRAMString("leds_2\r\n");
	delay_ms (DELAY);
	GPIO_ResetBits (GPIOD, GPIO_Pin_13);
	GPIO_SetBits (GPIOD, GPIO_Pin_14);
	UART2_PutRAMString("leds_3\r\n");
	delay_ms (DELAY);
	GPIO_SetBits (GPIOD, GPIO_Pin_15);
	GPIO_ResetBits (GPIOD, GPIO_Pin_14);
	UART2_PutRAMString("leds_4\r\n");
	delay_ms (DELAY);
	}
}
